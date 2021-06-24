/*
 *
 * ArduinOS - Processes source file
 * src/processes.cpp
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

#include <Arduino.h>
#include "common.h"
#include "processes.h"
#include "filesystem.h"
#include "memory.h"
#include "instruction_set.h"
#include "stack.h"

static int no_of_processes = 0;
static Process processes[AMOUNT_OF_FILES];

/**
 * Check if a process exists and if it is running.
 * 
 * @param proc_id the id of the process.
 * @return index of the process, or -1 if it doesn't exist, or is terminated.
 */
int checkRunning(int proc_id)
{
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].id == proc_id && processes[i].state != terminated) {
            return i;
        }
    }
    return -1;
}

/**
 * Change the status for a process.
 * 
 * @param proc_id the id of the process.
 * @param state the new state that the process should have.
 */
void changeProcessStatus(int proc_id, State state)
{
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].id == proc_id) {
            if (processes[i].state == state) {
                Serial.print(F("Error: cannot change status for process "));
                Serial.print(proc_id);
                Serial.print(F(". Process is already in this status."));
            }
            else
                processes[i].state = state;
        }
    }
}

/**
 * Execute one instruction of a process.
 * 
 * @param proc_id id of the process.
 */
static void execute(int index)
{
    uint8_t instruction = readPcByte(processes[index].pc++);
    uint8_t str_len = 0;

    switch(instruction) {
        case STOP:
            clearAllVars(processes[index].id);
            changeProcessStatus(processes[index].id, terminated);
            break;
        case CHAR:
        case INT:
        case FLOAT:
            for (uint8_t i = 0; i < instruction; i++) {
                pushByte(readPcByte(processes[index].pc++), processes[index].id);
            }
            pushByte(instruction, processes[index].id);
            break;
        case STRING:
            for (uint8_t b = readPcByte(processes[index].pc); b != '\0'; b = readPcByte(++processes[index].pc)) {
                pushByte(readPcByte(processes[index].pc), processes[index].id);
                str_len++;
            }
            pushByte(str_len, processes[index].id);
            pushByte(instruction, processes[index].id);
            break;
        case PRINT:
        case PRINTLN:
            Value v = popVal(processes[index].id);
            switch(v.type) {
                case CHAR:
                    if (instruction == PRINT) 
                        Serial.print((char)v.val.c);
                    else Serial.println((char)v.val.c);
                    break;
                case INT:
                    if (instruction == PRINT) 
                        Serial.print((int)v.val.i);
                    else Serial.println((int)v.val.i);
                    break;
                case FLOAT:
                    if (instruction == PRINT)
                        Serial.print((float)v.val.f);
                    else Serial.println((float)v.val.f);
                    break;
                case STRING:
                    if (instruction == PRINT)
                        Serial.print(v.val.s);
                    else Serial.println(v.val.s);
            }
            break;

    }
}

// Run all processes that are in the 'running' state.
void runProcesses()
{
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].state == running)
            execute(i);
    }
}

/**
 * Run a process by providing the process name.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void run(CommandArgs argv) 
{
    // parse filename argument
    char *file_name;
    if (strlen(argv.arg[0]) > 0) {
        file_name = strdup(argv.arg[0]);
    }
    else {
        Serial.println(F("Error: the filename argument is required."));
        return;
    }

    // check if there's space in process table
    if (no_of_processes == AMOUNT_OF_FILES) {
        Serial.println(F("Error: no space left in process table."));
        free(file_name);
        return;
    }

    // check if file exists in FAT
    int fat_entry_addr = findFATEntry(file_name);
    if (fat_entry_addr < 0) {
        Serial.print(F("Error: file \""));
        Serial.print(file_name);
        Serial.println(F("\" not found in filesystem."));
        free(file_name);
        return;
    }
    File file = readFATEntry(fat_entry_addr);

    // create entry in process table
    Process process = {0};
    strcpy(process.name, file.name);
    process.id = no_of_processes + 1;  // start id at 1 to allow for fail checks
    process.pc = file.addr;
    process.sp = 0;
    process.state = running;
    processes[no_of_processes++] = process;

    Serial.print(F("Process "));
    Serial.print(file_name);
    Serial.println(F(" is running."));

    free(file_name);    
}

/**
 * List all running processes.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void list(CommandArgs argv) 
{
    if (no_of_processes == 0) {
        Serial.println(F("No running processes."));
        return;
    }

    int processes_running = 0;
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].state != terminated) {
            Serial.print(processes[i].name);
            Serial.print(F(", id: "));
            Serial.print(processes[i].id);
            Serial.print(F(", status: "));
            Serial.println((char)processes[i].state);

            processes_running++;
        }
    }

    if (processes_running == 0) 
        Serial.println(F("No running processes."));
}


/**
 * Supend a process by providing the process id.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void suspend(CommandArgs argv) 
{
    int proc_id = atoi(argv.arg[0]);
    if (proc_id <= 0) {
        // no argument given, value below 0, atoi failed
        Serial.println(F("Error: invalid id provided."));
        return;
    }

    // check if process exists and not terminated
    int process = checkRunning(proc_id);
    if (process < 0) {
        Serial.print(F("Error: no process found with id "));
        Serial.println(proc_id);
        return;
    }

    // Change the status for the process
    changeProcessStatus(proc_id, paused);
}

/**
 * Resume a process by providing the process id.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void resume(CommandArgs argv) 
{
    int proc_id = atoi(argv.arg[0]);
    if (proc_id <= 0) {
        // no argument given, value below 0, atoi failed
        Serial.println(F("Error: invalid id provided."));
        return;
    }

    // check if process exists and not terminated
    int process = checkRunning(proc_id);
    if (process < 0) {
        Serial.print(F("Error: no process found with id "));
        Serial.println(proc_id);
        return;
    }

    // Change the status for the process
    changeProcessStatus(proc_id, running);
}

/**
 * Kill a process by providing the process id.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void kill(CommandArgs argv) 
{
    int proc_id = atoi(argv.arg[0]);
    if (proc_id <= 0) {
        // no argument given, value below 0, atoi failed
        Serial.println(F("Error: invalid id provided."));
        return;
    }

    // check if process exists and not terminated
    int process = checkRunning(proc_id);
    if (process < 0) {
        Serial.print(F("Error: no process found with id "));
        Serial.println(proc_id);
        return;
    }

    // remove all variables for this process
    clearAllVars(proc_id);

    // Change the status for the process
    changeProcessStatus(proc_id, terminated);
}

/**
 * Push a byte to the stack. This function does not focus on a specific type.
 * 
 * @param id process id of the process.
 */
void pushByte(uint8_t b, int id) 
{
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].id == id) {
            processes[i].stack[processes[i].sp++] = b;
        }
    }
}

/**
 * Pop one byte from the stack. This function does not focus on a specific type.
 * 
 * @param id process id of the process.
 * @return next byte from the stack, or 0 when there's nothing on the stack.
 */
uint8_t popByte(int id) 
{
    for (int i = 0; i < no_of_processes; i++) {
        if (processes[i].id == id) {
            return processes[i].stack[--processes[i].sp];
        }
    }
    return 0;
}