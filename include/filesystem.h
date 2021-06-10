/*
 *
 * ArduinOS - Filesystem header file
 * include/filesystem.h
 *
 * Copyright (C) 2021 Ricardo Steijn <0955903@hr.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"

#define NOF_PTR         0
#define FST_PTR         1

typedef struct {
    char name[FILENAME_SIZE];
    int addr;
    int size;
} File;

void initFileSystem();
int findFATEntry(const char *name);
File readFATEntry(int addr);

void store(CommandArgs argv);
void retrieve(CommandArgs argv);
void erase(CommandArgs argv);
void files(CommandArgs argv);
void freespace(CommandArgs argv);

// debug functions
void resetEEPROM();

#endif