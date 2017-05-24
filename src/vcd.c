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
 * \file      vcd.c
 * \brief     FAST5 decoder test application
 * \author    Bob Mittmann <bobmittmann@gmail.com>
 * \copyright 2017, Bob Mittmann
 */ 

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "debug.h"

struct vcd_var
{
	uint8_t type;
	uint8_t sizeoftype;
	uint8_t pos;
	char id;
	char name[30];
    double period;
    double start_time;
	uint32_t length;
	uint32_t allocsize;
	void * data;
};

#define VCD_VAR_MAX 32

struct vcd
{
	FILE * f;
	double timescale;
	unsigned int cnt;
	struct vcd_var var[VCD_VAR_MAX];
};

static const char * const month[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

struct vcd * vcd_create(const char * path, double timescale)
{
	time_t t;
	struct tm * tm;
	struct vcd * vcd;
	uint64_t ns;
	FILE * f;

	if (path != NULL) {
		if ((f = fopen(path, "w")) == NULL) {
			return NULL;
		};
	} else
		f = stdout;

	if ((vcd = (struct vcd *)malloc(sizeof(struct vcd))) == NULL) {
		if (f != stdout)
			fclose(f);
		return NULL;
	}

	vcd->f = f;
	vcd->timescale = timescale;

	t = time(NULL);
	tm = gmtime(&t);
	fprintf(f, "$date %s %d, %d %d:%02d:%02d $end\n", 
			month[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900, 
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	ns = round(timescale * 1000000000);
	fprintf(f, "$timescale %" PRIi64 " ns $end\n", ns);

	fprintf(f, "$scope module top $end\n");

	return vcd;
}

static const char id_lut[] = {
	'!',
	'@',
	'#',
	'$',
	'%',
	'^',
	'&',
	'*',
	'(',
	')',
	'+'
};

struct vcd_var * vcd_var_new(struct vcd * vcd, const char * name, 
							 double rate)
{
	struct vcd_var * var;
	int pos;

	pos = vcd->cnt;
	if (pos >= VCD_VAR_MAX)
		return NULL;

	vcd->cnt++;
	var = &vcd->var[pos];
	var->pos = pos;
	var->id = id_lut[pos];
	var->type = 1;
	var->sizeoftype = 2;

	strcpy(var->name, name);
	var->period = 1.0/rate;
	var->start_time = 0.0;
	var->length = 0;
	var->allocsize = 0;
	var->data = NULL;

	return var; 
}

int vcd_var_append(struct vcd_var * var, void * data, unsigned int len)
{
	unsigned int datasize;
	unsigned int allocsize;
	void * ptr;

	assert(var != NULL);
	assert(data != NULL);

	datasize = var->sizeoftype * len;

	if (var->allocsize == 0) {
		allocsize = datasize;
		ptr = malloc(allocsize);
		var->data  = ptr;
	} else {
		assert(var->allocsize != 0);
		allocsize = var->allocsize + datasize;
		ptr = realloc(var->data, allocsize);
		var->data  = ptr;
		ptr += datasize;
	}

	var->length += len;
	var->allocsize = allocsize;

	memcpy(ptr, data, datasize);

	return 0;
}

int vcd_close(struct vcd * vcd)
{
	struct vcd_var * var;
	int i;

	assert(vcd != NULL);
	assert(vcd->f != NULL);

	for (i = 0; i < vcd->cnt; ++i) {
		var = &vcd->var[i];
		if (var->data != NULL) {
			assert(var->allocsize != 0);
			free(var->data);
		}
	}

	fclose(vcd->f);

	free(vcd);

	return 0;
}

