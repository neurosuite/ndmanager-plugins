/***************************************************************************
                          tags.cpp  -  description
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

#include "tags.h"

namespace ndmanager{

extern const QString NEUROSCOPE = "neuroscope";
extern const QString PARAMETERS = "parameters";
extern const QString CREATOR = "creator";
extern const QString DOC__VERSION = "version";
extern const QString ACQUISITION = "acquisitionSystem";
extern const QString FIELD_POTENTIALS = "fieldPotentials";
extern const QString MISCELLANEOUS = "miscellaneous";
extern const QString VIDEO = "video";
extern const QString SAMPLING_RATES = "samplingRates";
extern const QString CHANNELS = "channels";
extern const QString UNITS = "units";
extern const QString ANATOMY = "anatomicalDescription";
extern const QString SPIKE = "spikeDetection";
extern const QString FILES = "files";
extern const QString PROGRAMS = "programs";

//Tags included in ACQUISITION
extern const QString BITS = "nBits";
extern const QString NB_CHANNELS = "nChannels";
extern const QString SAMPLING_RATE = "samplingRate";
extern const QString VOLTAGE_RANGE = "voltageRange";
extern const QString AMPLIFICATION = "amplification";
extern const QString OFFSET = "offset";

//Tags included in FILED_POTENTIALS
extern const QString LFP_SAMPLING_RATE = "lfpSamplingRate";

//Tags included in MISCELLANEOUS
extern const QString SCREENGAIN = "screenGain";
extern const QString TRACE_BACKGROUND_IMAGE = "traceBackgroundImage";

extern const QString SPIKES = "spikes";

//Tags included in VIDEO
extern const QString WIDTH = "width";
extern const QString HEIGHT = "height";
extern const QString ROTATE = "rotate";
extern const QString FLIP = "flip";
extern const QString VIDEO_IMAGE = "videoImage";
extern const QString POSITIONS_BACKGROUND = "positionsBackground";

//Tag included in SAMPLING_RATES
extern const QString EXTENSION_SAMPLING_RATE = "extensionSamplingRate";

//Tag included in FILES
extern const QString EXTENSION = "extension";
extern const QString CHANNEL_MAPPING = "channelMapping";
extern const QString ORIGINAL_CHANNELS = "originalChannels";

//Tag included in CHANNELS
extern const QString CHANNEL_COLORS = "channelColors";
extern const QString CHANNEL_OFFSET = "channelOffset";

extern const QString DEFAULT_OFFSET = "defaultOffset";

//Tags included in CHANNEL_DISPLAY
extern const QString CHANNEL = "channel";
extern const QString SKIP = "skip";
extern const QString COLOR = "color";
extern const QString ANATOMY_COLOR = "anatomyColor";
extern const QString SPIKE_COLOR = "spikeColor";

//Tag included in ANATOMY
extern const QString CHANNEL_GROUPS = "channelGroups";

//Tag included in CHANNEL_GROUPS
extern const QString GROUP = "group";

//Tag included in UNITS
extern const QString UNIT = "unit";
// Already defined in channel groups
// extern const QString GROUP = "group";
extern const QString CLUSTER = "cluster";
extern const QString STRUCTURE = "structure";
extern const QString TYPE = "type";
extern const QString ISOLATION_DISTANCE = "isolationDistance";
extern const QString QUALITY = "quality";
// Already defined in general info
// extern const QString NOTES = "notes";

//Tags included in SPIKE
extern const QString NB_SAMPLES = "nSamples";
extern const QString PEAK_SAMPLE_INDEX = "peakSampleIndex";
extern const QString NB_FEATURES = "nFeatures";

//Tag included in FILES
extern const QString FILE = "file";

extern const QString PROGRAM = "program";
extern const QString PARAMETER = "parameter";
extern const QString NAME = "name";
extern const QString STATUS = "status";
extern const QString VALUE = "value";
extern const QString HELP = "help";

extern const QString GENERAL = "generalInfo";
extern const QString DATE = "date";
extern const QString EXPERIMENTERS = "experimenters";
extern const QString DESCRIPTION = "description";
extern const QString NOTES = "notes";



}

