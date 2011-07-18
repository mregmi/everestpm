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

#include "platform.h"
#include "mbr.h"
#include "parttypes.h"



scan_system()
{
    char pathname[20];
    int ret;

    ndisks = get_ndisks();
    LOG_INFO("No of disks %d\n", ndisks);

    for(int i = 0; i < ndisks; i++)
    {
        get_nthdevice(pathname, ndisks);
        LOG_INFO("Scanning %s\n", pathname);
        ret = scan_partitions(pathname, i);
        if(ret < 0)
        {
            LOG_INFO("Scanning of %s failed: ", pathname);
            continue;
        }
    }
}

/* Reads The Extended Partitions */
int Ext2Read::scan_ebr(FileHandle handle, lloff_t base, int sectsize, int disk)
{
    unsigned char sector[SECTOR_SIZE];
    struct MBRpartition *part, *part1;
    Ext2Partition *partition;
    int logical = 4, ret;
    lloff_t  ebrBase, nextPart, ebr2=0;

    ebrBase = base;
    nextPart = base;
    while(1)
    {
        ret = read_disk(handle, sector, nextPart, 1, sectsize);
        if(ret < 0)
            return ret;

        if(ret < sectsize)
        {
            LOG("Error Reading the EBR \n");
            return -1;
        }
        part = pt_offset(sector, 0);
        LOG("index %d ID %X size %Ld \n", logical, part->sys_ind, get_nr_sects(part));

        /*if((part->sys_ind == 0x05) || (part->sys_ind == 0x0f))
        {
            // special case. ebr has extended partition with offset to another ebr.
            ebr2 += get_start_sect(part);
            nextPart = (ebr2 + ebrBase);
            continue;
        }*/
        part1 = pt_offset(sector, 1);
        ebr2 = get_start_sect(part1);
        nextPart = (ebr2 + ebrBase);

        logical++;
        if(part1->sys_ind == 0)
            break;
    }
    return logical;
}
