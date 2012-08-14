/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
** Copyright (C) 2008 Michaël Zugaro <michael.zugaro@college-de-france.fr>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

// #include	"common.h"

#define _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE64
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <samplerate.h>

#define	BUFFER_LEN	 1024

static void usage_exit (const char *progname) ;
static long sample_rate_convert (FILE *infile, FILE *outfile, int converter, double src_ratio, int channels, double * gain) ;
static double apply_gain (float * data, long frames, int channels, double max, double gain) ;

typedef struct
{	long			frames ;		/* Used to be called samples.  Changed to avoid confusion. */
	int			samplerate ;
	int			channels ;
	int			format ;
	int			sections ;
	int			seekable ;
} SF_INFO;

int
main (int argc, char *argv [])
{	FILE	*infile, *outfile ;
	SF_INFO sfinfo ;

	long	count ;
	double		src_ratio = -1.0, gain = 1.0 ;
	int			k, converter;
	int			channels;

	channels = -1;
	double samplerate = -1,new_sample_rate = -1;

// 	if (argc != 5 && argc != 7 && argc != 8)
// 		usage_exit (argv [0]) ;

	/* Set default converter. */
	converter = SRC_SINC_FASTEST ;

	// CLI: process_sincresample -n nChannels {-f frequency,frequency | -r ratio} input output

	for (k=1 ; k < argc ; k++)
	{	if (strcmp (argv [k], "-f") == 0)
		{	k ++ ;
			char *token;
			token = strtok(argv [k],",:-");
			samplerate = atof(token);
			token = strtok(NULL,",:-");
			new_sample_rate = atof (token) ;
			}
		else if (strcmp (argv [k], "-n") == 0)
		{	k ++ ;
			channels = atof (argv [k]) ;
		}
		else if (strcmp (argv [k], "-r") == 0)
		{	k ++ ;
			src_ratio = atof (argv [k]) ;
			}
		else if (strcmp (argv [k], "-c") == 0)
		{	k ++ ;
			converter = atoi (argv [k]) ;
			}
		else if (strcmp (argv [k], "-h") == 0)
			usage_exit (argv [0]) ;
		else if (argv [k][0] == '-')
		{
			printf("error: unknown option %s (type '%s -h' for details)",argv[k],argv[0]);
			exit(1);
		}
		else break;
	} ;
	if (argc-k != 2)
	{	printf ("error: missing input and/or output files.\n") ;
		exit (1) ;
		} ;

	if (new_sample_rate <= 0 && src_ratio <= 0.0)
	{	printf ("error: missing sampling rates or frequency ratio.\n") ;
		exit (1) ;
		} ;

	if (src_get_name (converter) == NULL)
	{	printf ("error: bad converter number.\n") ;
		exit (1) ;
		} ;

	if (strcmp (argv [argc - 2], argv [argc - 1]) == 0)
	{	printf ("error: input and output file names are the same.\n") ;
		exit (1) ;
		} ;

	if (new_sample_rate > 0 & samplerate > 0) src_ratio = (1.0 * new_sample_rate) / samplerate ;
	else if (src_is_valid_ratio (src_ratio)) new_sample_rate = (int) floor (samplerate * src_ratio) ;
	else
	{	printf ("error: cannot determine new sampling rate.\n") ;
		exit (1) ;
		} ;

	if (src_is_valid_ratio (src_ratio) == 0)
	{	printf ("error: sampling rate change out of valid range.\n") ;
		exit (1) ;
		} ;

	if (channels < 0)
	{	printf ("error: incorrect or missing number of channels.\n") ;
		exit (1) ;
		} ;

	if (! (infile = fopen (argv [argc - 2], "rb")))
	{	printf ("error: cannot open input file '%s'\n", argv [argc - 2]) ;
		exit (1) ;
		} ;

	/* Delete the output file length to zero if already exists. */
	remove (argv [argc - 1]) ;

	if ((outfile = fopen (argv [argc - 1], "wb")) == NULL)
	{	printf ("error: cannot open output file '%s'\n", argv [argc - 1]) ;
		fclose (infile) ;
		exit (1) ;
		} ;

	printf ("Input File     : %s\n", argv [argc - 2]) ;
	printf ("Channels       : %d\n", channels) ;
	if (samplerate != -1) printf ("Sampling Rate  : %f\n", samplerate) ;

	printf ("Output file    : %s\n", argv [argc - 1]) ;
	if (new_sample_rate != -1) printf ("Sampling Rate  : %f\n", new_sample_rate) ;

	printf ("Freq Ratio     : %f\n", src_ratio) ;
	printf ("Converter      : %s\n", src_get_name (converter)) ;

	do
		count = sample_rate_convert (infile, outfile, converter, src_ratio, channels, &gain) ;
	while (count < 0) ;

	printf ("Output Frames  : %ld\n", (long) count) ;

	fclose (infile) ;
	fclose (outfile) ;

	return 0 ;
} /* main */

/*==============================================================================
*/

static long
sample_rate_convert (FILE *infile, FILE *outfile, int converter, double src_ratio, int channels, double * gain)
{	short short_buffer [BUFFER_LEN*channels] ;
	float input [BUFFER_LEN*channels] ;
	float output [BUFFER_LEN*channels] ;

	SRC_STATE	*src_state ;
	SRC_DATA	src_data ;
	int			error,i ;
	double		max = 0.0 ;
	long	output_count = 0 ;

	fseek (infile, 0, SEEK_SET) ;
	fseek (outfile, 0, SEEK_SET) ;

	/* Initialize the sample rate converter. */
	if ((src_state = src_new (converter, channels, &error)) == NULL)
	{	printf ("\n\nerror: src_new() failed: %s.\n\n", src_strerror (error)) ;
		exit (1) ;
		} ;

	src_data.end_of_input = 0 ; /* Set this later. */

	/* Start with zero to force load in while loop. */
	src_data.input_frames = 0 ;
	src_data.data_in = input ;

	src_data.src_ratio = src_ratio ;

	src_data.data_out = output ;
	src_data.output_frames = BUFFER_LEN;

	long   s,nMarks = 0,maxNMarks = 50,chunk = 0,nChunks;
	printf("Resampling     : 0%% ");
	for ( i = 0 ; i < maxNMarks ; i++ ) printf(".");
	printf(" 100%%\b\b\b\b\b");
	for ( i = 0 ; i < maxNMarks ; i++ ) printf("\b");
	fseek(infile,0,SEEK_END);
	s = ftell(infile);
	nChunks = s/(BUFFER_LEN*channels*sizeof(short));
	rewind(infile);

	while (1)
	{
/*		chunk++;*/
		if ( ((float)chunk/(float)nChunks) > ((float)nMarks)/((float)maxNMarks) )
		{
			int nMarksToAdd = (int) (((float)(chunk+1)/(float)nChunks*(float)maxNMarks) - (float) nMarks);
			for ( i = 0 ; i < nMarksToAdd ; i++ ) fprintf(stdout,"#");
			nMarks += nMarksToAdd;
			fflush(stdout);
		}
		/* If the input buffer is empty, refill it. */
		if (src_data.input_frames == 0)
		{
			chunk++;
			src_data.input_frames = fread(short_buffer,sizeof(short),BUFFER_LEN*channels,infile);
			src_data.input_frames /= channels;
		 	src_short_to_float_array(short_buffer,input,BUFFER_LEN*channels);

			src_data.data_in = input ;

			/* The last read will not be a full buffer, so snd_of_input. */
			if (src_data.input_frames < BUFFER_LEN)
				src_data.end_of_input = 1 ;
			} ;
// 			printf("---\ndata_in = %d\ndata_out = %d\ninput_frames = %d\noutput_frames = %d\ninput_frames_used = %d\noutput_frames_gen = %d\nend_of_input = %d\nsrc_ratio = %f\n",
// 			src_data.data_in,src_data.data_out,src_data.input_frames,src_data.output_frames,src_data.input_frames_used,src_data.output_frames_gen,src_data.end_of_input,src_data.src_ratio);
// ((SRC_PRIVATE*)src_state)->last_position = 1;
		if ((error = src_process (src_state, &src_data)))
		{	printf ("\nerror: %s\n", src_strerror (error)) ;
			exit (1) ;
			} ;
// 		printf("AFTER  last_pos = %d\n",((SRC_PRIVATE*)src_state)->last_position);
		/* Terminate if done. */
		if (src_data.end_of_input && src_data.output_frames_gen == 0)
			break ;

//// 		max = apply_gain (src_data.data_out, src_data.output_frames_gen, channels, max, *gain) ;

		/* Write output. */
		src_float_to_short_array(output,short_buffer,BUFFER_LEN*channels);
//  		printf("read = %d, used = %d, generated = %d\n",src_data.input_frames,src_data.input_frames_used,src_data.output_frames_gen);
		fwrite (short_buffer, sizeof(short), src_data.output_frames_gen*channels, outfile) ;

		output_count += src_data.output_frames_gen ;

		src_data.data_in += src_data.input_frames_used * channels ;
		src_data.input_frames -= src_data.input_frames_used ;
		} ;
// 		printf("#\n");
	src_state = src_delete (src_state) ;

// 	if (max > 1.0)
// 	{	*gain = 1.0 / max ;
// 		printf ("\nOutput has clipped. Restarting conversion to prevent clipping.\n\n") ;
// 		output_count = 0 ;
// 		return -1 ;
// 		} ;
	printf("\n",chunk,nChunks);
	return output_count ;
} /* sample_rate_convert */

static double
apply_gain (float * data, long frames, int channels, double max, double gain)
{
	long k ;

	for (k = 0 ; k < frames * channels ; k++)
	{	data [k] *= gain ;

		if (fabs (data [k]) > max)
			max = fabs (data [k]) ;
		} ;

	return max ;
} /* apply_gain */

static void
usage_exit (const char *progname)
{	const char	*cptr ;
	int		k ;

	if ((cptr = strrchr (progname, '/')) != NULL)
		progname = cptr + 1 ;

	if ((cptr = strrchr (progname, '\\')) != NULL)
		progname = cptr + 1 ;

	printf ("usage: %s [-c converter] {-f f1,f2 | -r ratio} -n channels input output\n"
		" -n channels      number of channels in input file\n"
		" -c converter     converter ID (see below)\n"
		" -f f1,f2         frequencies for input and output files\n"
		" -r ratio         frequency ratio (output/input)\n"
		" input            input file\n"
		" output           output file\n"
		, progname) ;

	puts (
		"Available converters include"
		) ;

	for (k = 0 ; (cptr = src_get_name (k)) != NULL ; k++)
		printf (" %d   %s\n", k, cptr) ;

	exit (0) ;
} /* usage_exit */

/*==============================================================================
*/

