/*
 * LL(1) Predictive Parser Table Generator and RDP Generator
 * 
 * This file is part of bobcall.
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
 * \file      fast5.c
 * \brief     FAST5 decoder test application
 * \author    Bob Mittmann <bobmittmann@gmail.com>
 * \copyright 2017, Bob Mittmann
 */ 

#define __FAST5_I__
#include "fast5-i.h"
#include <assert.h>

struct fast5
{
	hid_t file_id;
	hid_t dataset_id;
};

struct fast5 * fast5_open(const char * path)
{
	hid_t file_id;
	struct fast5 * f5;

	assert(path != NULL);

	if ((file_id = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0){
		return NULL;
	};

	if ((f5 = (struct fast5 *)malloc(sizeof(struct fast5))) != NULL) {
		f5->file_id = file_id;
	}

	return f5;
}

int fast5_close(struct fast5 * f5)
{
	assert(f5 != NULL);
	assert(f5->file_id >= 0);


	H5Fclose(f5->file_id);

	free(f5);

	return 0;
}

int fast5_stats(struct fast5 * f5)
{
	assert(f5 != NULL);
	assert(f5->file_id >= 0);

	return 0;
}

