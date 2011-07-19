/**
 * Everest Partition Manager
 * File: platform_win32.c
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

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#include <fcntl.h>

#include "everestpm.h"
#include "platform.h"

#define DEVICE	"\\\\.\\PhysicalDrive0"

FileHandle open_disk(const char *path, int *sect_size)
{
    HANDLE handle;


    handle = CreateFileA(path, GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         0, 0 );

    if(handle == INVALID_HANDLE_VALUE)
    {
        LOG_INFO("Error Opening %s. Error Code %X\n", path, GetLastError());
        return handle;
    }


    return handle;
}

int get_ndisks()
{
    HANDLE hDevice;               // handle to the drive to be examined
    int ndisks = 0;
    int index = 0;
    char path[] = {"\\\\.\\PhysicalDrive0"};

    do {
        hDevice = CreateFileA(path, // drive to open
                              GENERIC_READ,
                              FILE_SHARE_READ,  // share mode
                              NULL,    // default security attributes
                              OPEN_EXISTING,  // disposition
                              0,       // file attributes
                              NULL);   // don't copy any file's attributes

        if(hDevice == INVALID_HANDLE_VALUE)
        {
            LOG_INFO("Error Opening %s. Error Code %X\n", path, GetLastError());
            break;
        }

        CloseHandle(hDevice);
        ndisks++;
        path[strlen(path) - 1] = (char)('0' + ndisks);
    }while(hDevice != INVALID_HANDLE_VALUE);

    return ndisks;
}

void close_disk(FileHandle handle)
{
    CloseHandle(handle);
}

int read_disk(FileHandle hnd, void *ptr, lloff_t sector, int nsects, int sectorsize)
{
    lloff_t offset;
    DWORD rd, len;
    DWORD low;
    LONG high;
    BOOL ret;

    offset = sector * sectorsize;

    low = (DWORD)(offset & 0xFFFFFFFF);
    high = (DWORD)((offset >> 32)& 0xFFFFFFFF);

    low = SetFilePointer(hnd, low, &high, FILE_BEGIN);

    len = nsects * sectorsize;
    ret = ReadFile(hnd, ptr, len, &rd, NULL);
    if(!ret)
        return -1;

    return rd;
}


int write_disk(FileHandle hnd, void *ptr, lloff_t sector, int nsects, int sectorsize)
{
    lloff_t offset;
    DWORD rd, len;
    DWORD low;
    LONG high;
    BOOL ret;

    offset = sector * sectorsize;

    low = (DWORD)(offset & 0xFFFFFFFF);
    high = (DWORD)((offset >> 32)& 0xFFFFFFFF);

    low = SetFilePointer(hnd, low, &high, FILE_BEGIN);

    len = nsects * sectorsize;
    ret = ReadFile(hnd, ptr, len, &rd, NULL);
    if (!ret)
        return -1;

    return rd;
}

void
get_diskinfo(struct disk_info *dsk, int i)
{
    HANDLE hDevice;               // handle to the drive to be examined
    DISK_GEOMETRY_EX geom;
    BOOL bResult;
    DWORD junk;
    char path[] = {"\\\\.\\PhysicalDrive0"};

    path[strlen(path) - 1] = '0' + i;
    hDevice = CreateFileA(path,     // drive to open
                GENERIC_READ,
                FILE_SHARE_READ,    // share mode
                NULL,               // default security attributes
                OPEN_EXISTING,      // disposition
                0,                  // file attributes
                NULL);              // don't copy any file's attributes

    if (hDevice == INVALID_HANDLE_VALUE) {
        LOG_INFO("Error Opening %s. Error Code %X\n", path, GetLastError());
        return;
    }
    strcpy(dsk->device_file, path);
    bResult = DeviceIoControl(handle,  // device we are querying
                              IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,  // operation to perform
                              NULL, 0, // no input buffer, so pass zero
                              &geom, sizeof(geom),  // output buffer
                              &junk, // discard count of bytes returned
                              (LPOVERLAPPED) NULL);  // synchronous I/O

    if((!bResult) || (geom.Geometry.BytesPerSector < SECTOR_SIZE))
        dsk->sector_size = SECTOR_SIZE;
    else
        dsk->sector_size = geom.Geometry.BytesPerSector;
    dsk->size = geom.DiskSize;

}

#endif
