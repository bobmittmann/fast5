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
 * \file      f5dump.c
 * \brief     FAST5 decoder test application
 * \author    Robinson Mittmann <bobmittmann@gmail.com>
 * \copyright 2016, Bob Mittmann
 */ 

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <libgen.h>
#include <stdbool.h>

#include "config.h"
#include "fast5.h"

int verbose = 0;

void usage(FILE * f, char * prog)
{
	fprintf(f, "Usage: %s [OPTION...] FILE\n", prog);
	fprintf(f, "FAST5 decoder test.\n");
	fprintf(f, "\n");
	fprintf(f, "  -?     \tShow this help message\n");
	fprintf(f, "  -v[v]  \tVerbosity level\n");
	fprintf(f, "\n");
}

void version(char * prog)
{
	fprintf(stderr, "%s\n", PACKAGE_STRING);
	fprintf(stderr, "(C)Copyright, Bob Mittmann.\n");
	exit(1);
}


int main(int argc,  char **argv)
{
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	char * prog;
	struct fast5 * f5;

	char * filename; /* name of fast5 input file */
	int c;

	/* the prog name start just after the last lash */
	if ((prog = (char *)basename(argv[0])) == NULL)
		prog = argv[0];

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?v")) > 0) {
		switch (c) {
		case 'V':
			version(prog);
			break;

		case '?':
			usage(stdout, prog);
			return 0;

		case 'v':
			verbose++;
			break;

		default:
			fprintf(stderr, "%s: invalid option %s\n", prog, optarg);
			return 1;
		}
	}

	if (optind != (argc - 1)) {
		fprintf(stderr, "%s: missing filename.\n\n", prog);
		usage(stderr, prog);
		return 2;
	}

	filename = argv[optind];

	f5 = fast5_open(filename);

	fast5_stats(f5);

	fast5_close(f5);

	if (verbose)
		fprintf(stdout, " - finished without errors.\n");

	return 0;
}

