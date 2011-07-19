/**
 * Everest Partition Manager
 * File: everestpm.h
 **/
/**
 * Copyright (C) 2005 by Manish Regmi   (regmi dot manish at gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#include "platform.h"
#include "mbr.h"

/* Error Codes */
#define INVALID_TABLE   1

#define MAX_DEVICE_NAME_LEN 256 /* this will suffice */
#define DEVICE_INFO_LEN     128 /* should be enough */
struct partition_info {

};

struct disk_info {
    uint32_t type;  /* Partition Table Type */
    uint32_t flag;  /* Flag */
    FileHandle fd;  /* File Descriptor */
    lloff_t size;   /* Total size of disk */
    int sector_size;

    char device_file[MAX_DEVICE_NAME_LEN]; /* Device File */
    char vendor[DEVICE_INFO_LEN];
    union {
        struct MBRpartition MBR; /* Cached boot record */
    };
    struct partition_info *partition;
};

struct system_info {
    int ndisks;
    struct disk_info *disk;
};

#define LOG_INFO    epm_log

int epm_log(const char *msg, ...);
