/**
 * Everest Partition Manager
 * File: everestpm.c
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

#include "everestpm.h"
#include "mbr.h"

struct system_info system;

void scan_system()
{
    struct disk_info *dsk;
    static char pathname[256];
    int ret;

    system.ndisks = get_ndisks();
    LOG_INFO("No of disks %d\n", system.ndisks);
    system.disk = malloc(sizeof(struct disk_info) * system.ndisks);
    for(int i = 0; i < ndisks; i++)
    {
        dsk = &system.disk[i];
        get_diskinfo(dsk, i);
        LOG_INFO("Scanning %s\n", dsk->device_file);
        ret = scan_partitions(dsk);
        if(ret < 0)
        {
            LOG_INFO("Scanning of %s failed: ", dsk->device_file);
            continue;
        }
    }
}
