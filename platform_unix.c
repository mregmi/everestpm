/**
 * Everest Partition Manager
 * File: platform_unix.c
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

#ifdef __APPLE__ || __unix__

#include "platform.h"

#ifdef __APPLE__
#define DEVICE "/dev/rdisk0"
#elif __linux__
#define DEVICE "/dev/sda"
#endif

void
get_diskinfo(struct disk_info *dsk, int i)
{
    char path = DEVICE;

    path[strlen(path) - 1] = '0' + i;
    dsk->fd = open(path, O_RDONLY);
    if (dsk->fd < 0) {
        LOG_INFO("Error Opening %s. Error Code %X\n", path, strerr());
        break;
    }
    strcpy(dsk->device_file, path);
    dsk->sector_size = SECTOR_SIZE
}

int get_ndisks()
{
    int fd, ndisks = 0;
    char dev_file = DEVICE;
    do {
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            LOG_INFO("Error Opening %s. Error Code %X\n", path, strerr());
            break;
        }

        close(fd);
        ndisks++;
        path[strlen(path) - 1] = (char)('0' + ndisks);
    } while (fd > 0);

    return ndisks;
}

void
close_disk(FileHandle fd)
{
    close(fd);
}

int
read_disk(FileHandle handle, void *ptr, lloff_t sector, int nsects,
              int sectorsize)
{
    lloff_t offset;
    ssize_t rd;
    size_t len;

    offset = sector * sectorsize;
    lseek64(handle, offset, SEEK_SET);

    len = nsects * sectorsize;
    rd = read(handle, ptr, len);
    if(rd != len)
        return -1;

    return rd;
}


int
write_disk(FileHandle handle, void *ptr, lloff_t sector, int nsects,
               int sectorsize)
{
    lloff_t offset;
    ssize_t rd;
    size_t len;

    offset = sector * sectorsize;
    lseek64(handle, offset, SEEK_SET);

    len = nsects * sectorsize;
    rd = read(handle, ptr, len);
    if(rd != len)
        return -1;

    return rd;
}

#endif
