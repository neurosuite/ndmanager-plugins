/***************************************************************************
 *   Copyright (C) 2004-2011 by Michael Zugaro                             *
 *   michael.zugaro@college-de-france.fr                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef CUSTOM_TYPES
#define CUSTOM_TYPES

#include <stdlib.h>

// Neuralynx data types are defined as a custom types (easier to read and maintain)
typedef int16_t   Data;
typedef int64_t   Time;
typedef int32_t   Frequency;
typedef int32_t	Coordinates;

// Constants
#define MAX_N_RESTARTS	512
#define PROGRESS_MAX_N_CHARS	6
// ncs
#define NCS_N_SAMPLES_PER_RECORD 512
#define NCS_MAX_N_CHANNELS 512
#define NCS_FILE_HEADER_SIZE 16*1024
#define NCS_RECORD_HEADER_SIZE 20
#define NCS_MAX_FREQUENCY_ERROR 2 // Allowable error in sampling frequency (Hz)
#define NCS_MAX_OVERLAP 100 // Allowable overlap between successive records (µs)
// nvt
#define NVT_FILE_HEADER_SIZE 16*1024
#define NVT_N_TARGETS 50
#define NVT_N_DWPOINTS 400
#define NVT_FRAME_SIZE 1828
#define NVT_MASK_X 4095 // = 2^12-1
#define NVT_MASK_Y 268369920 // = (2^12-1)*2^16
#define NVT_SHIFT_Y 65536 // = (2^12-1)*2^16
#define NVT_MASK_RED 1073741824 // = 2^30
#define NVT_MASK_GREEN 536870912 // = 2^29
#define NVT_MASK_BLUE 268435456 // = 2^28
// nev
#define NEV_FILE_HEADER_SIZE 16*1024
#define NEV_RECORD_SIZE 184
// smi
#define SMI_MAX_N_FILES 512
#define SMI_MATCH_PATTERN "<SYNC"
#define SMI_PATTERN_BEFORE "ENUSCC>"
#define SMI_PATTERN_AFTER "</SYNC>"

#endif
