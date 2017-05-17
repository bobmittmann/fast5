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

/* Opaque structures */
struct fast5;

#ifdef __cplusplus
extern "C" {
#endif

struct fast5 * fast5_open(const char * path);

int fast5_close(struct fast5 * f5);

int fast5_stats(struct fast5 * f5);
		
#ifdef __cplusplus
}
#endif

#endif /* __FAST5_H__ */

