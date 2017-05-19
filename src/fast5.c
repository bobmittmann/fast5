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

#define DEBUG_LEVEL DBG_TRACE
#include "fast5-i.h"
#include <assert.h>
#include <fast5.h>
#include <libgen.h>

struct fast5
{
	struct fast5_info info;
	bool has_raw;
	bool has_sequences;
	hid_t file;
};

struct fast5 * fast5_open(const char * path)
{
	struct fast5 * f5;
	hid_t file;
	hid_t attr;
	float ver;

	assert(path != NULL);

	if ((file = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0){
		return NULL;
	};

	/* Check if attribute /file_version exists in root group. */
	if ((attr = H5Aopen(file, "file_version", H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Attribute \"/file_version\" not found!");
		return NULL;
	}

	H5Aread(attr, H5T_NATIVE_FLOAT, &ver);
	H5Aclose(attr);

	DBG(DBG_INFO, "file_version = %0f", ver);

	if ((f5 = (struct fast5 *)malloc(sizeof(struct fast5))) != NULL) {
		f5->file = file;
	}

	strcpy(f5->info.filename, basename((char *)path));

	f5->info.version.major = ver;
	ver -= f5->info.version.major;
	f5->info.version.minor = ver * 100;

	/* Check if group /UniqueGlobalKey exists in the file. */
	if (H5Lexists(file, "/UniqueGlobalKey", H5P_DEFAULT) <= 0) {
		DBG(DBG_WARNING, "Group \"/UniqueGlobalKey\" not found!");
		return NULL;
	}

	/* Check if group /Analyses exists in the file. */
	if (H5Lexists(file, "/Analyses", H5P_DEFAULT) <= 0) {
		DBG(DBG_WARNING, "Group \"/Analyses\" not found!");
		return NULL;
	}

	/* Check if group /Sequences exists in the file. */
	if (H5Lexists(file, "/Sequences", H5P_DEFAULT) <= 0) {
		DBG(DBG_INFO, "Group \"/Sequences\" not found!");
		f5->has_sequences = true;
	} else
		f5->has_sequences = false;

	/* Check if group /Raw exists in the file. */
	if (H5Lexists(file, "/Raw", H5P_DEFAULT) <= 0) {
		DBG(DBG_INFO, "Group \"/Raw\" not found!");
		f5->has_raw = true;
	} else
		f5->has_raw = false;

	return f5;
}

int fast5_close(struct fast5 * f5)
{
	assert(f5 != NULL);
	assert(f5->file >= 0);

	H5Fclose(f5->file);

	free(f5);

	return 0;
}

int fast5_info(struct fast5 * f5, struct fast5_info * info)
{
	assert(f5 != NULL);
	assert(f5->file >= 0);

	memcpy(info, &f5->info, sizeof(struct fast5_info));

	return 0;
}

/* -------------------------------------------------------------------------
 * Raw signals
 * ------------------------------------------------------------------------- */ 

/* Get the link name for the raw reads group */
static int fast5_raw_get_name(struct fast5 * f5, char * name)
{
	H5G_info_t  ginfo;
	hid_t group;
	int ret;
	int i;

	/* Check if group /Raw/Reads exists in the file. */
	if (H5Lexists(f5->file, "/Raw", H5P_DEFAULT) <= 0)
		return -1;

	/* Check if group /Raw/Reads exists in the file. */
	if (H5Lexists(f5->file, "/Raw/Reads", H5P_DEFAULT) <= 0)
		return -1;

	if ((group = H5Gopen(f5->file, "/Raw/Reads", H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Can't access group \"/Raw/Reads\"!");
		return group;
	}

	if ((ret = H5Gget_info(group, &ginfo)) < 0) {
		DBG(DBG_WARNING, "Can't access group \"/Raw/Reads\"!");
		return ret;
	}

	if (ginfo.nlinks == 0) {
		DBG(DBG_WARNING, "Empty group!");
		return -1;
	}

	if (ginfo.nlinks > 1) {
		DBG(DBG_WARNING, "Too many raw sub-groups!");
	}

	i = 0;

	H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, 
					   name, FAST5_OBJ_PATH_MAX, H5P_DEFAULT);
	H5Gclose(group);

	return 0;
}

int fast5_raw_read_info(struct fast5 * f5, struct fast5_raw * info)
{
	char gpath[FAST5_OBJ_PATH_MAX];
	char name[FAST5_OBJ_PATH_MAX];
	hid_t dataset;  
	hid_t dspace;
	hid_t group;
	hid_t attr;
	hsize_t dims[16];
	int ndims;
	int ret;

	assert(f5 != NULL);
	assert(f5->file >= 0);
	assert(info != NULL);

	memset(info, 0, sizeof(struct fast5_raw));

	if ((ret = fast5_raw_get_name(f5, name)) < 0)
		return ret;

	snprintf(info->dataset, FAST5_OBJ_PATH_MAX, "/Raw/Reads/%s/Signal", name);
	DBG(DBG_INFO, "Raw signal: %s", info->dataset);

	snprintf(gpath, FAST5_OBJ_PATH_MAX, "/Raw/Reads/%s", name);
	if ((group = H5Gopen(f5->file, gpath, H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Cant open \"%s\" group!", gpath);
		return -1;
	}

	if ((attr = H5Aopen(group, "duration", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_ULONG, &info->duration);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "median_before", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->median_before);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "read_id", H5P_DEFAULT)) >= 0) {
		/* read a fixed length string */
		hid_t type = H5Tcopy(H5T_C_S1);
		H5Tset_size(type, FAST5_UUID_MAX);
		H5Aread(attr, type, info->read_id);
		H5Aclose(attr);
		H5Tclose(type);
	} else {
		DBG(DBG_WARNING, "Can't read attribute: \"read_id\"!");
	}

	if ((attr = H5Aopen(group, "read_number", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_ULONG, &info->read_number);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "start_mux", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_LONG, &info->start_mux);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "start_time", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_ULLONG, &info->start_time);
		H5Aclose(attr);
	}

	/* Open dataset. */
	dataset = H5Dopen2(f5->file, info->dataset, H5P_DEFAULT);
	/* GEt the dataset's dataspace. */
	dspace = H5Dget_space(dataset);
	/* Get dimensions */
	ndims = H5Sget_simple_extent_ndims(dspace);
	H5Sget_simple_extent_dims(dspace, dims, NULL);
	DBG(DBG_INFO, "ndims=%d dims[0]=%d", ndims, (int)dims[0]);

	info->length = dims[0];

	/* Close the dataspace. */
	H5Sclose(dspace);  
	/* Close the dataset. */
	H5Dclose(dataset);

	H5Gclose(group);

	return 0;
}

int fast5_raw_read(struct fast5 * f5, int16_t * raw, size_t len)
{
	char name[FAST5_OBJ_PATH_MAX];
	char path[FAST5_OBJ_PATH_MAX];
	hid_t dataset;  
	herr_t status;
	hid_t dataspace_id;
	hid_t memspace_id;
	hsize_t dimsm[2];
	hsize_t count[2];              /* size of subset in the file */
	hsize_t offset[2];             /* subset offset in the file */
	hsize_t stride[2];
	hsize_t block[2];
	int ret;

	assert(f5 != NULL);
	assert(f5->file >= 0);
	assert(raw != NULL);

	if ((ret = fast5_raw_get_name(f5, name)) < 0)
		return ret;

	snprintf(path, FAST5_OBJ_PATH_MAX, "/Raw/Reads/%s/Signal", name);
	DBG(DBG_INFO, "Raw signal: %s", path);

	/* Open an existing dataset. */
	dataset = H5Dopen2(f5->file, path, H5P_DEFAULT);

	/* Specify size and shape of subset to write. */

	offset[0] = 0;
	count[0]  = len;
	stride[0] = 1;
	block[0] = 1;

	/* Create memory space with size of subset. Get file dataspace
	   and select subset from file dataspace. */

	dimsm[0] = len;
	memspace_id = H5Screate_simple(1, dimsm, NULL);

	dataspace_id = H5Dget_space(dataset);
	H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, 
						offset, stride, count, block);

	status = H5Dread(dataset, H5T_NATIVE_SHORT, 
					 memspace_id, dataspace_id, 
					 H5P_DEFAULT, raw);

	H5Sclose(memspace_id);
	H5Sclose(dataspace_id);
	/* Close the dataset. */
	H5Dclose(dataset);

	return status;
}

/* -------------------------------------------------------------------------
 * Events detection
 * ------------------------------------------------------------------------- */ 

/* Get the link name for the events detection reads group */
static int fast5_events_read_dirname(struct fast5 * f5, char * path)
{
	char dir[FAST5_OBJ_PATH_MAX];
	char name[FAST5_OBJ_PATH_MAX];
	H5G_info_t  ginfo;
	hid_t group;
	int i;

	/* Check if group /Analyses/Reads exists in the file. */
	if (H5Lexists(f5->file, "/Analyses", H5P_DEFAULT) <= 0) {
		DBG(DBG_WARNING, "Group \"Analyses\" don't exist!");
		return -1;
	}

	sprintf(dir, "/Analyses/%s", "EventDetection_000");

	/* Check if group /Analyses/EventDetection_000 exists in the file. */
	if (H5Lexists(f5->file, dir, H5P_DEFAULT) <= 0) {
		DBG(DBG_WARNING, "Group \"%s\" don't exist!", dir);
		return -1;
	}

	strcat(dir, "/Reads");

	/* Check if group /Analysis/EventDetection_000/Reads exists in the file. */
	if (H5Lexists(f5->file, dir, H5P_DEFAULT) <= 0) {
		DBG(DBG_WARNING, "Group \"%s\" don't exist!", dir);
		return -1;
	}

	if ((group = H5Gopen(f5->file, dir, H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Can't access group \"%s\"!", dir);
		return group;
	}

	H5Gget_info(group, &ginfo);

	if (ginfo.nlinks == 0) {
		DBG(DBG_WARNING, "Empty group!");
		return -1;
	}

	if (ginfo.nlinks > 1) {
		DBG(DBG_WARNING, "Too many raw sub-groups!");
	}

	i = 0;

	H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, 
					   name, FAST5_OBJ_PATH_MAX, H5P_DEFAULT);

	sprintf(path, "%s/%s", dir, name);

	H5Gclose(group);

	return 0;
}

int fast5_events_info(struct fast5 * f5, struct fast5_events_info * info)
{
	char path[FAST5_OBJ_PATH_MAX];
	hid_t dataset;  
	hid_t dspace;
	hid_t group;
	hid_t attr;
	hsize_t dims[16];
	int ndims;
	int ret;

	assert(f5 != NULL);
	assert(f5->file >= 0);
	assert(info != NULL);

	memset(info, 0, sizeof(struct fast5_events_info));

	if ((ret = fast5_events_read_dirname(f5, path)) < 0)
		return ret;

	snprintf(info->dataset, FAST5_OBJ_PATH_MAX, "%s/Events", path);
	DBG(DBG_INFO, "Event detection events: %s", info->dataset);

	if ((group = H5Gopen(f5->file, path, H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Cant open \"%s\" group!", path);
		return -1;
	}

	if ((attr = H5Aopen(group, "duration", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_ULONG, &info->duration);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "median_before", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->median_before);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "read_id", H5P_DEFAULT)) >= 0) {
		/* read a fixed length string */
		hid_t type = H5Tcopy(H5T_C_S1);
		H5Tset_size(type, FAST5_UUID_MAX);
		H5Aread(attr, type, info->read_id);
		H5Aclose(attr);
		H5Tclose(type);
	}

	if ((attr = H5Aopen(group, "read_number", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_ULONG, &info->read_number);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "scaling_used", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_LLONG, &info->scaling_used);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "start_mux", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_LONG, &info->start_mux);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "start_time", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->start_time);
		H5Aclose(attr);
	}

	H5Gclose(group);

	/* Open dataset. */
	dataset = H5Dopen2(f5->file, info->dataset, H5P_DEFAULT);
	/* GEt the dataset's dataspace. */
	dspace = H5Dget_space(dataset);
	/* Get dimensions */
	ndims = H5Sget_simple_extent_ndims(dspace);
	H5Sget_simple_extent_dims(dspace, dims, NULL);
	DBG(DBG_INFO, "ndims=%d dims[0]=%d", ndims, (int)dims[0]);

	info->length = dims[0];

	/* Close the dataspace. */
	H5Sclose(dspace);  
	/* Close the dataset. */
	H5Dclose(dataset);

	return 0;
}

int fast5_events_read(struct fast5 * f5, struct fast5_event * event, 
					  size_t len)
{
	char path[FAST5_OBJ_PATH_MAX];
	char datasetname[FAST5_OBJ_PATH_MAX];
	hid_t dataset;  
	herr_t status;
	hid_t dataspace_id;
	hid_t memspace_id;
	hsize_t dimsm[2];
	hsize_t count[2];              /* size of subset in the file */
	hsize_t offset[2];             /* subset offset in the file */
	hsize_t stride[2];
	hsize_t block[2];
	hid_t type;
	int ret;

	assert(f5 != NULL);
	assert(f5->file >= 0);
	assert(event != NULL);

	if ((ret = fast5_events_read_dirname(f5, path)) < 0)
		return ret;

	snprintf(datasetname, FAST5_OBJ_PATH_MAX, "%s/Events", path);

	/* Open dataset. */
	dataset = H5Dopen2(f5->file, datasetname, H5P_DEFAULT);

	/* Specify size and shape of subset to read. */
	offset[0] = 0;
	count[0]  = len;
	stride[0] = 1;
	block[0] = 1;

	/*
	 * Create the memory data type.
	 */
	type = H5Tcreate(H5T_COMPOUND, sizeof(struct fast5_event));
	H5Tinsert(type, "start", HOFFSET(struct fast5_event, start), 
			  H5T_NATIVE_LLONG);
	H5Tinsert(type, "length", HOFFSET(struct fast5_event, length), 
			  H5T_NATIVE_LLONG);
	H5Tinsert(type, "mean", HOFFSET(struct fast5_event, mean), 
			  H5T_NATIVE_DOUBLE);
	H5Tinsert(type, "stdv", HOFFSET(struct fast5_event, stdv), 
			  H5T_NATIVE_DOUBLE);
	H5Tinsert(type, "variance", HOFFSET(struct fast5_event, variance), 
			  H5T_NATIVE_DOUBLE);

	/* Create memory space with size of subset. Get file dataspace
	   and select subset from file dataspace. */
	dimsm[0] = len;
	memspace_id = H5Screate_simple(1, dimsm, NULL);

	dataspace_id = H5Dget_space(dataset);
	H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, 
						offset, stride, count, block);

	status = H5Dread(dataset, type, 
					 memspace_id, dataspace_id, 
					 H5P_DEFAULT, event);

	H5Sclose(memspace_id);
	H5Sclose(dataspace_id);
	/* Close the dataset. */
	H5Dclose(dataset);

	return status;
}

int fast5_channel_id(struct fast5 * f5, struct fast5_channel_id * info)
{
	hid_t group;
	hid_t attr;
	char * s;

	assert(f5 != NULL);
	assert(f5->file >= 0);
	assert(info != NULL);

	memset(info, 0, sizeof(struct fast5_channel_id));

	if ((group = H5Gopen(f5->file, "/UniqueGlobalKey/channel_id", 
						 H5P_DEFAULT)) < 0) {
		DBG(DBG_WARNING, "Can't access group \"/UniqueGlobalKey/channel_id\"!");
		return group;
	}

	if ((attr = H5Aopen(group, "channel_number", H5P_DEFAULT)) >= 0) {
		/* read a variable length string */
		hid_t type = H5Tcopy(H5T_C_S1);
		H5Tset_size(type, H5T_VARIABLE);
		H5Aread(attr, type, &s);
		strncpy(info->channel_number, s, FAST5_CHAN_NUM_MAX);
		info->channel_number[FAST5_CHAN_NUM_MAX] = '\0';
		free(s);
		H5Aclose(attr);
		H5Tclose(type);
	} else {
		DBG(DBG_WARNING, "Can't read attribute: \"read_id\"!");
	}

	if ((attr = H5Aopen(group, "digitisation", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->digitisation);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "offset", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->offset);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "range", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->range);
		H5Aclose(attr);
	}

	if ((attr = H5Aopen(group, "sampling_rate", H5P_DEFAULT)) >= 0) {
		H5Aread(attr, H5T_NATIVE_DOUBLE, &info->sampling_rate);
		H5Aclose(attr);
	}

	H5Gclose(group);

	return 0;
}


int fast5_stats(struct fast5 * f5)
{
	assert(f5 != NULL);
	assert(f5->file >= 0);

	return 0;
}

