/*
 * fast5 - FAST5 decoder libary 
 * 
 * This file is part of libfast5.
 *
 * Ell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*! 
 * \file      fast5.h
 * \brief     FAST5 library public API
 * \author    Bob Mittmann <bobmittmann@gmail.com>
 * \copyright 2017, Bob Mittmann
 */ 


#ifndef __FAST5_H__
#define __FAST5_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

/* Opaque structures */
struct fast5;

struct fast5_info {
	char filename[PATH_MAX];
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
};

#define FAST5_CHAN_NUM_MAX 127

struct fast5_channel_id {
	char channel_number[FAST5_CHAN_NUM_MAX + 1];
	double digitisation;
	double offset;
	double range;
	double sampling_rate;
};

#define FAST5_UUID_MAX 63
#define FAST5_OBJ_PATH_MAX 127

/* Raw read info */
struct fast5_raw {
	char dataset[FAST5_OBJ_PATH_MAX + 1];
	uint32_t duration;
	double median_before;
	char read_id[FAST5_UUID_MAX + 1];
	uint32_t read_number;
	int32_t start_mux;
	uint64_t start_time;
	size_t length;
};

/* Event detection info */
struct fast5_events_info {
	char dataset[FAST5_OBJ_PATH_MAX + 1];
	uint32_t duration;
	double median_before;
	char read_id[FAST5_UUID_MAX + 1];
	uint32_t read_number;
	int64_t scaling_used;
	int32_t start_mux;
	double start_time;
	size_t length;
};

struct fast5_event {
	int64_t start;
	int64_t length;
	double mean;
	double stdv;
	double variance;
};

#ifdef __cplusplus
extern "C" {
#endif

struct fast5 * fast5_open(const char * path);

int fast5_close(struct fast5 * f5);

int fast5_info(struct fast5 * f5, struct fast5_info * info);

int fast5_stats(struct fast5 * f5);
		
int fast5_raw_read_info(struct fast5 * f5, struct fast5_raw * info);

int fast5_raw_read(struct fast5 * f5, int16_t * raw, size_t len);

int fast5_events_info(struct fast5 * f5, struct fast5_events_info * info);

int fast5_events_read(struct fast5 * f5, struct fast5_event * event, 
					  size_t len);

int fast5_channel_id(struct fast5 * f5, struct fast5_channel_id * info);

#ifdef __cplusplus
}
#endif

#endif /* __FAST5_H__ */

