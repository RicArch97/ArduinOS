/*
 *
 * ArduinOS - Memory header file
 * include/memory.h
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

#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>

#define MAX_VAR_AMOUNT  25
#define MEM_SIZE        256

typedef struct {
    char name;
    uint8_t type;
    uint8_t size;
    uint8_t addr;
    int proc_id;
} Variable;

void setVar(char name, int proc_id);
void getVar(char name, int proc_id);
void clearVar(char name, int proc_id);
void clearAllVars(int proc_id);

#endif