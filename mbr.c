/**
 * Everest Partition Manager
 * File: mbr.c
 **/
/**
 * Copyright (C) 2005, 2010 by Manish Regmi   (regmi dot manish at gmail.com)
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
/**
  * This file contains implementation of scanning and retrieving
  * partition information. For now we only support MBR style partitions.
  **/

#include <dirent.h>

#include "everestpm.h"
#include "platform.h"
#include "mbr.h"
#include "parttypes.h"



/* Reads The Extended Partitions */
int scan_ebr(struct disk_info *dsk, struct partition_info *ext)
{
    static unsigned char sector[SECTOR_SIZE];
    struct MBRpartition *part, *part1;
    struct partition_info *current = NULL, *pt;
    int logical = 4, ret;
    lloff_t  ebrBase, nextPart, ebr2=0;

    ebrBase = ext->base;
    nextPart = ext->base;
    while (1) {
        ret = read_disk(dsk->fd, sector, nextPart, 1, dsk->sector_size);
        if (ret < 0)
            return ret;

        if (ret < dsk->sector_size) {
            LOG_INFO("Error Reading the EBR \n");
            return -1;
        }
        part = pt_offset(sector, 0);
        LOG_INFO("index %d ID %X size %Ld \n", logical, part->sys_ind, get_nr_sects(part));

        if (is_extended(part->sys_ind)) {
            // special case. ebr has extended partition with offset to another ebr.
            ebr2 += get_start_sect(part);
            nextPart = (ebr2 + ebrBase);
            continue;
        }
        part1 = pt_offset(sector, 1);
        ebr2 = get_start_sect(part1);
        nextPart = (ebr2 + ebrBase);

        pt = malloc(sizeof(struct partition_info));
        if (!pt) {
            LOG_INFO("Allocation Error\n");
        }
        memset(pt, 0, sizeof(struct partition_info));
        if (!current) {
            current = pt;
            ext->ext = pt;
        } else {
            current->next = pt;
            current = current->next;
        }
        pt->fstype = part->sys_ind;
        pt->base = get_start_sect(part);
        pt->size = get_nr_sects(part) * dsk->sector_size;
        logical++;
        if (part1->sys_ind == 0)
            break;
    }
    return logical;
}

/* Scans The partitions */
int scan_partitions(struct disk_info *dsk)
{
    static unsigned char sector[SECTOR_SIZE];
    struct MBRpartition *part;
    struct partition_info *current = NULL;
    int sector_size;
    int ret, i;

    ret = read_disk(dsk->fd, sector, 0, 1, dsk->sector_size);
    if(ret < 0)
        return ret;

    if(ret < sector_size)
    {
        LOG_INFO("Error Reading the MBR on %s \n", path);
        return -1;
    }

    if(!valid_part_table_flag(sector))
    {
        LOG_INFO("Partition Table Error on %s\n", dsk->device_file);
        LOG_INFO("Invalid End of sector marker");
        return -INVALID_TABLE;
    }

    /* First Scan primary Partitions */
    for(i = 0; i < 4; i++)
    {
        part = pt_offset(sector, i);
        if((part->sys_ind != 0x00) || (get_nr_sects(part) != 0x00))
        {
            LOG_INFO("index %d ID %X size %Ld \n", i, part->sys_ind, get_nr_sects(part));

            if(!dsk->partition) {
                current = malloc(sizeof(struct partition_info));
                memset(current, 0, sizeof(struct partition_info));
                dsk->partition = current;
            } else {
                current->next = malloc(sizeof(struct partition_info));
                memset(current->next, 0, sizeof(struct partition_info));
                current = current->next;
            }
            current->fstype = part->sys_ind;
            current->base = get_start_sect(part);
            current->size = get_nr_sects(part) * dsk->sector_size;
            if(is_extended(part->sys_ind)) {
                current->flag = PARTITION_TYPE_EXTENDED;
                scan_ebr(dsk, current);
            } else {
                current->flag = PARTITION_TYPE_PRIMARY;
            }
        }
    }

    return 0;
}

int add_loopback(const char *file)
{
    int ret;

    ndisks++;
    ret = scan_partitions((char *)file, ndisks);

    return 0;
}

