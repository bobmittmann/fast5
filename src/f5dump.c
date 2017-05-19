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
#include <inttypes.h>

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
	fprintf(f, "  -d     \tDump\n");
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
	struct fast5_info info;
	struct fast5_raw raw_read;
	struct fast5_events_info events_info;
	struct fast5_channel_id channel_id;
	char * path; /* fast5 input file */
	int c;
	int16_t * raw;
	int cnt;
	int i;
	bool dump = false;

	/* the prog name start just after the last lash */
	if ((prog = (char *)basename(argv[0])) == NULL)
		prog = argv[0];

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?vd")) > 0) {
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

		case 'd':
			dump = true;
			break;

		default:
			fprintf(stderr, "%s: invalid option %s\n", prog, optarg);
			return 1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "%s: missing filename.\n\n", prog);
		usage(stderr, prog);
		return 2;
	}

	while (optind < argc) {
		path = argv[optind++];

		if ((f5 = fast5_open(path)) == NULL) {
			fprintf(stderr, "%s: Not a FAST5 file!\n", prog);
			return 3;
		}

	//	fast5_stats(f5);
	
		fast5_info(f5, &info);
		printf("      file_name: %s\n", info.filename); 
		printf("   file_version: %d.%d\n", info.version.major, 
			   info.version.minor);

		if (fast5_channel_id(f5, &channel_id) < 0) {
			fprintf(stderr, "%s: channel_id error!\n", prog);
			return 3;
		}

		if (verbose) {
			printf(" channel_number: %s\n", channel_id.channel_number);
			printf("   digitisation: %f\n", channel_id.digitisation);
			printf("         offset: %f\n", channel_id.offset);
			printf("          range: %f\n", channel_id.range);
			printf("  sampling_rate: %f\n", channel_id.sampling_rate);
		}

		if (fast5_raw_read_info(f5, &raw_read) < 0) {
			fprintf(stderr, "%s: raw info error!\n", prog);
		} else if (verbose) {
			printf("    Raw dataset: %s\n", raw_read.path);
			printf("       duration: %u\n", raw_read.duration);
			printf("  median_before: %f\n", raw_read.median_before);
			printf("        read_id: %s\n", raw_read.read_id);
			printf("    read_number: %u\n", raw_read.read_number);
			printf("      start_mux: %d\n", raw_read.start_mux);
			printf("     start_time: %" PRIu64 "\n", raw_read.start_time);
		}

		if (fast5_events_info(f5, &events_info) < 0) {
			fprintf(stderr, "%s: events info error!\n", prog);
		} else if (verbose) {
			printf("  Event dataset: %s\n", events_info.path);
			printf("       duration: %u\n", events_info.duration);
			printf("  median_before: %f\n", events_info.median_before);
			printf("        read_id: %s\n", events_info.read_id);
			printf("    read_number: %u\n", events_info.read_number);
			printf("   scaling_used: %" PRIi64 "\n", events_info.scaling_used);
			printf("      start_mux: %d\n", events_info.start_mux);
			printf("     start_time: %f\n", events_info.start_time);
		}

		if (dump) {
			cnt = raw_read.duration;
			raw = malloc(cnt*sizeof(int16_t));;

			if (fast5_raw_read(f5, raw, cnt) < 0) {
				fprintf(stderr, "%s: raw data read error!\n", prog);
				return 3;
			}

			for (i = 0; i < cnt; ++i) {
				printf("%d\n", raw[i]);
			}

			free(raw);
		}

		if (verbose) {
			printf("\n");
		}

		fast5_close(f5);
	}

	return 0;
}

