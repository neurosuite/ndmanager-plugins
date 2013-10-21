/***************************************************************************
                          tags.h  -  description
                             -------------------
    begin                : Wed Mar 31 2004
    copyright            : (C) 2004 by Lynn Hazan
    email                : lynn.hazan.myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TAGS_H
#define TAGS_H

// include files for QT
#include <qstring.h>

/**
  * This class contains the XML tags used in the xml files.
  *@author Lynn Hazan
  */

/* @namespace ndmanager*/
namespace ndmanager{

/**Tag for the neuroscope element.*/
extern const QString NEUROSCOPE;
/**Tag for the parameter file root element.*/
extern const QString PARAMETERS;
/**Tag for the creator attribut of the parameter file root element.*/
extern const QString CREATOR;
/**Tag for the version attribute of the root element.*/
extern const QString DOC__VERSION;
/**Tag for the acquisition element.*/
extern const QString ACQUISITION;
/**Tag for the fieldPotentials element.*/
extern const QString FIELD_POTENTIALS;
/**Tag for the miscellaneous element.*/
extern const QString MISCELLANEOUS;
/**Tag for the video element.*/
extern const QString VIDEO;
/**Tag for the samplingRate element.*/
extern const QString SAMPLING_RATES;
/**Tag for the channels element.*/
extern const QString CHANNELS;
/**Tag for the anatomy element.*/
extern const QString ANATOMY;
/**Tag for the spike element.*/
extern const QString SPIKE;
/**Tag for the unit element.*/
extern const QString UNITS;
/**Tag for the file element.*/
extern const QString FILES;
/**Tag for the programs element.*/
extern const QString PROGRAMS;
/**Tag for the general Info element.*/
extern const QString GENERAL;


//Tags included in ACQUISITION
/**Tag for the bits element included in the acquisition element.*/
extern const QString BITS;
/**Tag for the nbChannels element included in the acquisition element.*/
extern const QString NB_CHANNELS;
/**Tag for the samplingRate element included in the acquisition element.*/
extern const QString SAMPLING_RATE;
/**Tag for the voltageRange element included in the acquisition element.*/
extern const QString VOLTAGE_RANGE;
/**Tag for the amplification element included in the acquisition element.*/
extern const QString AMPLIFICATION;
/**Tag for the offset element included in the acquisition element.*/
extern const QString OFFSET;

//Tags included in FILED_POTENTIALS
/**Tag for the lfpSamplingRate element included in the filedPotentials element.*/
extern const QString LFP_SAMPLING_RATE;

//Tags included in MISCELLANEOUS
/**Tag for the screenGain element included in the miscellaneous element.*/
extern const QString SCREENGAIN;
/**Tag for the traceBackgroundImage element included in the miscellaneous element.*/
extern const QString TRACE_BACKGROUND_IMAGE;


/**Tag for the spikes element.*/
extern const QString SPIKES;

//Tags included in VIDEO
/**Tag for the width element included in the video element.*/
extern const QString WIDTH;
/**Tag for the height element included in the video element.*/
extern const QString HEIGHT;
/**Tag for the rotate element included in the video element.*/
extern const QString ROTATE;
/**Tag for the flip element included in the video element.*/
extern const QString FLIP;
/**Tag for the video image element included in the file element.*/
extern const QString VIDEO_IMAGE;
/**Tag for the positions backgroung element included in the file element.*/
extern const QString POSITIONS_BACKGROUND;


/**Tag for the extension element.*/
extern const QString EXTENSION;
extern const QString CHANNEL_MAPPING;
extern const QString ORIGINAL_CHANNELS;

/**Tag for the channelColors.*/
extern const QString CHANNEL_COLORS;
/**Tag for the channelOffset element.*/
extern const QString CHANNEL_OFFSET;
/**Tag for the defaultOffset element.*/
extern const QString DEFAULT_OFFSET;


/**Tag for the channel element.*/
extern const QString CHANNEL;
/**Tag for the skip attribute of the channel element.*/
extern const QString SKIP;
/**Tag for the colors element.*/
extern const QString COLOR;
/**Tag for the anatomyColors element.*/
extern const QString ANATOMY_COLOR;
/**Tag for the spikeColors element.*/
extern const QString SPIKE_COLOR;

/**Tag for the channelGroups element.*/
extern const QString CHANNEL_GROUPS;

/**Tag for the groups element.*/
extern const QString GROUP;

/**Tag for the unit element.*/
extern const QString UNIT;
extern const QString CLUSTER;
extern const QString STRUCTURE;
extern const QString TYPE;
extern const QString ISOLATION_DISTANCE;
extern const QString QUALITY;
extern const QString NOTES;

/**Tag for the channel element.*/
extern const QString CHANNEL;

//Tags included in SPIKE
/**Tag for the nbSamples element included in the spike element.*/
extern const QString NB_SAMPLES;
/**Tag for the peakSAmpleIndex element included in the spike element.*/
extern const QString PEAK_SAMPLE_INDEX;
/**Tag for the nFeatures element included in the spike element.*/
extern const QString NB_FEATURES;


/**Tag for the file element included in the files element.*/
extern const QString FILE;


/**Tag for the date element included in the generalInfo element.*/
extern const QString DATE;
extern const QString EXPERIMENTERS;
extern const QString DESCRIPTION ;
extern const QString NOTES;


/**Tag for the program element.*/
extern const QString PROGRAM;
extern const QString PARAMETER;
extern const QString NAME;
extern const QString STATUS;
extern const QString VALUE;
extern const QString HELP;
}

#endif
