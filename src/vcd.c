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

#include "debug.h"

struct vcd
{
	FILE * f;
	double timescale;
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

	return vcd;
}

int vcd_close(struct vcd * vcd)
{
	assert(vcd != NULL);
	assert(vcd->f != NULL);

	fclose(vcd->f);

	free(vcd);

	return 0;
}

