/*
 *
 * ArduinOS - Processes header file
 * include/processes.h
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

#ifndef PROCESSES_H
#define PROCESSES_H

#include "common.h"
#include "stack.h"

typedef enum {
    running = 'r',
    paused = 'p',
    terminated = 0
} State;

typedef struct {
    char name[FILENAME_SIZE];
    int id;
    State state;
    int pc;
    uint8_t sp;
    int fp;
    uint8_t stack[STACKSIZE];
} Process;

void runProcesses();
int checkRunning(int proc_id);
void changeProcessStatus(int proc_id, State status);

void run(CommandArgs argv);
void list(CommandArgs argv);
void suspend(CommandArgs argv);
void resume(CommandArgs argv);
void kill(CommandArgs argv);

void pushByte(uint8_t b, int id);
uint8_t popByte(int id);

#endif