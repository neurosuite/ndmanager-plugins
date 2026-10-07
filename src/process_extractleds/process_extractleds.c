/***************************************************************************
                          process_extractleds.c
                             -------------------
    begin                : Wed May 22 2002
    copyright            : (C) 2002 by Ken Harris
    email                : kdharris@andromeda.rutgers.edu
    copyright            : (C) 2002-2015 by Michaël Zugaro
    email                : michael.zugaro@college-de-france.fr
    copyright            : (C) 2026 by Florian Franzen
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
	Finds bright spots (LEDs) in every frame of a video and writes their
	statistics to <video basename>.spots in the current directory, one line
	per spot:

		frame, pixels, mean x, mean y, sd x, sd y, mean Y, mean U, mean V

	The last line is "<last frame> -1 -1 -1 -1 -1 -1 -1 -1", so that the
	number of frames is known even if the last frame has no spot.

	Up to version 1.4.14 this program was a modified copy of avplay from
	libav 11.2 (Fabrice Bellard). It now decodes the video with the FFmpeg
	libraries directly; the spot detection below is unchanged.

	Note on the chroma columns: the original program received the chroma
	planes of the player's overlay in swapped order, so the columns labelled
	"mean Cr" and "mean Cb" have always contained the U (Cb) and V (Cr)
	averages respectively. This order is kept for compatibility.
 ***************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

/* ------------------------------------------------------------------------ */
/* Spot detection (Ken Harris, Michaël Zugaro)                               */
/* ------------------------------------------------------------------------ */

#define sqr(x) ((x)*(x))
#define STRLEN 10000

const int FillWidth = 3; /* >1 allows the fill to skip over subthreshold points */
int Thresh = 90; /* intensity threshold (luminance) - can be overridden by -t */
uint8_t *PixLabel; /* 0: below threshold. 1: above threshold, unassigned. 2: already assigned */
int simulate = 0; /* if true, do not write output to disk */
int FrameNo = 0; /* Frame number */
FILE *SpotFp; /* spots file */

/* structure that keeps track of spot statistics for calculating mean color etc */
typedef struct SpotStats_s {
    int nPoints;
    float xSum, ySum, x2Sum, y2Sum; /* they are floats so you can divide them by int and get int */
    float LumSum, CrSum, CbSum;
} SpotStats;

static void ResetSpotStats(SpotStats *s) {
    s->nPoints  = s->xSum = s->ySum = s->x2Sum = s->y2Sum = s->LumSum = s->CrSum = s->CbSum = 0;
}

/* recursive fill algorithm, collects spot stats as it goes */
static void FloodFill(uint8_t *PixLabel, uint8_t *lum, uint8_t *Cr, uint8_t *Cb, int width,
                int height, int x, int y, SpotStats *s) {
    int dx, dy;

    if (x<0 || x>=width || y<0 || y>=height) return;

    if (PixLabel[x+y*width]==1) {
        /* increment stats structure */
        s->nPoints++;
        s->xSum+=x; s->ySum+=y;
        s->x2Sum+=sqr(x); s->y2Sum+=sqr(y);

        /* color averaging */
        s->LumSum+=lum[x+y*width];
        s->CrSum+=Cr[((short)x/2)+((short)y/2)*width/2];
        s->CbSum+=Cb[((short)x/2)+((short)y/2)*width/2];

        /* continue fill */
        PixLabel[x+y*width]=2;
        for (dx=-FillWidth; dx<=FillWidth; dx++) for (dy=-FillWidth; dy<=FillWidth; dy++) {
            if (abs(dx)+abs(dy) <= FillWidth) {
                FloodFill(PixLabel, lum, Cr, Cb, width, height, x+dx, y+dy, s);
            }
        }
    }
}

static void ExtractLEDs(uint8_t *lum, uint8_t *Cr, uint8_t *Cb, int width, int height)
{
    int x, y;
    SpotStats s;

    /* threshold image (the loop used to run y up to width, reading past the frame) */
    for (x=0; x<width; x++) for (y=0; y<height; y++) {
        PixLabel[x+y*width] = (lum[x+y*width]>Thresh);
    }

    /* loop through looking for spots and filling them */
    for (x=0; x<width; x++) for (y=0; y<height; y++) {
        if (PixLabel[x+y*width]==1) {
            /* if it is above threshold but not already assigned, fill the spot */
            ResetSpotStats(&s);
            FloodFill(PixLabel, lum, Cr, Cb, width, height, x, y, &s);

            /* frameno, nPoints, mean X, mean Y, sd X, sd Y, mean Lum, mean Cr, mean Cb */
            if ( ! simulate )
            {
                fprintf(SpotFp, "%d %d %f %f %f %f %f %f %f\n", FrameNo, s.nPoints,
                    s.xSum/s.nPoints, s.ySum/s.nPoints,
                    sqrt( s.x2Sum/s.nPoints - sqr(s.xSum/s.nPoints) ),
                    sqrt( s.y2Sum/s.nPoints - sqr(s.ySum/s.nPoints) ),
                    s.LumSum/s.nPoints, s.CrSum/s.nPoints, s.CbSum/s.nPoints);
            }
        }
    }
    FrameNo++;
}

/* ------------------------------------------------------------------------ */
/* Command line, video decoding                                              */
/* ------------------------------------------------------------------------ */

static void usage(const char *program)
{
    fprintf(stderr,
        "usage: %s [options] video\n"
        "  -t threshold  detection threshold, 0 to 255 (default 90)\n"
        "  -n            simulate, i.e. do not write to disk\n"
        "  -i            show video rate and duration using stream header\n"
        "  -i2           show video rate and duration after counting frames\n"
        "                (slower but required for incorrect headers)\n"
        "  -hide         accepted for compatibility; there is no video display\n",
        program);
}

/* <video basename> with the last extension replaced by .spots */
static void spots_file_name(const char *video, char *out, size_t size)
{
    const char *base = strrchr(video, '/');
    base = base ? base + 1 : video;
    snprintf(out, size, "%s", base);
    char *dot = strrchr(out, '.');
    if (dot) *dot = '\0';
    strncat(out, ".spots", size - strlen(out) - 1);
}

static double frame_rate(AVStream *st)
{
    return (double) st->avg_frame_rate.num / st->avg_frame_rate.den;
}

int main(int argc, char **argv)
{
    const char *input = NULL;
    int info = 0, info2 = 0;

    for (int i = 1; i < argc; ++i) {
        const char *a = argv[i];
        if (!strcmp(a, "-t") && i + 1 < argc) {
            char *end;
            long t = strtol(argv[++i], &end, 10);
            if (*end || t < 0 || t > 255) {
                fprintf(stderr, "invalid threshold (0 to 255)\n");
                return 1;
            }
            Thresh = (int) t;
        } else if (!strcmp(a, "-n")) {
            simulate = 1;
        } else if (!strcmp(a, "-i")) {
            info = 1;
        } else if (!strcmp(a, "-i2")) {
            info2 = 1;
        } else if (!strcmp(a, "-hide") || !strcmp(a, "-nodisp")) {
            /* nothing to hide */
        } else if (!strcmp(a, "-h") || !strcmp(a, "--help")) {
            usage(argv[0]);
            return 0;
        } else if (a[0] == '-') {
            fprintf(stderr, "unknown option: %s\n", a);
            usage(argv[0]);
            return 1;
        } else {
            input = a;
        }
    }
    if (!input) {
        usage(argv[0]);
        return 1;
    }

    av_log_set_level(AV_LOG_ERROR);

    AVFormatContext *fmt = NULL;
    if (avformat_open_input(&fmt, input, NULL, NULL) < 0) {
        fprintf(stderr, "%s: could not open file\n", input);
        return 1;
    }
    if (avformat_find_stream_info(fmt, NULL) < 0) {
        fprintf(stderr, "%s: could not read stream information\n", input);
        return 1;
    }
    const AVCodec *codec = NULL;
    int vs = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    if (vs < 0) {
        fprintf(stderr, "%s: no video stream\n", input);
        return 1;
    }
    AVStream *st = fmt->streams[vs];

    if (info) {
        fprintf(stdout, "VIDEO Average Sampling Rate (Hz)   %f\n", frame_rate(st));
        fprintf(stdout, "VIDEO Duration (s) [Header]        %lf\n", (double) fmt->duration / AV_TIME_BASE);
        return 0;
    }
    if (info2) {
        AVPacket *pkt = av_packet_alloc();
        int64_t n = 0;
        while (av_read_frame(fmt, pkt) == 0) {
            if (pkt->stream_index == vs) n++;
            av_packet_unref(pkt);
        }
        fprintf(stdout, "VIDEO Average Sampling Rate (Hz)   %f\n", frame_rate(st));
        fprintf(stdout, "VIDEO Number of Frames             %ld\n", (long) n);
        fprintf(stdout, "VIDEO Duration (s) [COMPUTED]      %lf\n", (double) n / frame_rate(st));
        return 0;
    }

    AVCodecContext *dec = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(dec, st->codecpar);
    if (avcodec_open2(dec, codec, NULL) < 0) {
        fprintf(stderr, "%s: could not open video decoder\n", input);
        return 1;
    }

    const int width = dec->width, height = dec->height;
    /* Convert every frame to planar YUV 4:2:0, as the original player's
       overlay did, with tightly packed planes. */
    struct SwsContext *sws = NULL;
    uint8_t *planes[4];
    int strides[4];
    if (av_image_alloc(planes, strides, width, height, AV_PIX_FMT_YUV420P, 1) < 0) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    PixLabel = calloc((size_t) width * height, 1);

    char spots[STRLEN];
    spots_file_name(input, spots, sizeof(spots));
    if (!simulate) {
        SpotFp = fopen(spots, "w");
        if (!SpotFp) {
            fprintf(stderr, "%s: could not create file\n", spots);
            return 1;
        }
    }

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    const double time_base = av_q2d(st->time_base);
    int flushing = 0;
    while (1) {
        if (!flushing) {
            if (av_read_frame(fmt, pkt) < 0) {
                flushing = 1;
                avcodec_send_packet(dec, NULL);
            } else {
                if (pkt->stream_index == vs) avcodec_send_packet(dec, pkt);
                av_packet_unref(pkt);
            }
        }
        int r;
        while ((r = avcodec_receive_frame(dec, frame)) == 0) {
            /* frame size and format are constant within a stream */
            if (!sws)
                sws = sws_getContext(frame->width, frame->height, frame->format,
                                     width, height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(sws, (const uint8_t * const *) frame->data, frame->linesize, 0,
                      frame->height, planes, strides);
            /* U is passed as "Cr" and V as "Cb", see note at the top */
            ExtractLEDs(planes[0], planes[1], planes[2], width, height);
            if (frame->best_effort_timestamp != AV_NOPTS_VALUE) {
                fprintf(stdout, "%7.1f\b\b\b\b\b\b\b", frame->best_effort_timestamp * time_base);
                fflush(stdout);
            }
            av_frame_unref(frame);
        }
        if (r == AVERROR_EOF) break;
        if (r != AVERROR(EAGAIN)) {
            fprintf(stderr, "%s: error while decoding\n", input);
            break;
        }
    }

    printf("\nDone!\n");
    if (!simulate) {
        /* one empty frame at the end, so that the number of frames is known
           even if there is no spot in the last frame */
        fprintf(SpotFp, "%d -1 -1 -1 -1 -1 -1 -1 -1\n", FrameNo - 1);
        fclose(SpotFp);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    sws_freeContext(sws);
    av_freep(&planes[0]);
    free(PixLabel);
    avcodec_free_context(&dec);
    avformat_close_input(&fmt);
    return 0;
}
