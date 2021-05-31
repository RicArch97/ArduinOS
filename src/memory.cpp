/*
 *
 * ArduinOS - Memory source file
 * src/memory.cpp
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
#include "instruction_set.h"
#include "memory.h"
#include "stack.h"

static int no_of_vars = 0;
static Variable variables[MAX_VAR_AMOUNT];
static uint8_t memory[MEM_SIZE];

// sort memory table based on the begin address of variable data.
static void sortMemoryTable() {
    for (int c = 0; c < (no_of_vars * sizeof(Variable)); c++) {
        for (int n = c + sizeof(Variable); n < (no_of_vars * sizeof(Variable)); n++) {
            if (variables[c].addr > variables[n].addr) {
                Variable temp = variables[c];
                variables[c] = variables[n];
                variables[n] = temp;
            }
        }
    }
}

/**
 * Check if there is enough space left in the RAM.
 * 
 * @param size the size of the variable that needs to be allocated.
 * @return address pointer where the memory can be written, or -1 for errors.
 */
static int checkMemoryTable(uint8_t size) 
{
   if (no_of_vars == 0) return 0;

   // sort memory table based on begin address
   if (no_of_vars > 1) sortMemoryTable();
}

/**
 * Pop a variable from the stack and save it in memory.
 * 
 * @param name name (1 byte) of the variable.
 * @param proc_id the process id of the process this variable belongs to. 
 */
void setVar(char name, int proc_id) 
{
    // check if maximum reached
    if (no_of_vars == MAX_VAR_AMOUNT) {
        Serial.println(F("Error: max amount in variables in RAM reached."));
        return;
    }

    // check if name and proc id exist
    for (int e = 0; e < (sizeof(variables) / sizeof(Variable)); e++) {
        if (name == variables[e].name && proc_id == variables[e].proc_id) {
            // remove element from array by moving over the index to be removed
            variables[e] = variables[--no_of_vars];
        }
    }

    // check the type for a string
    uint8_t type = popByte();
    uint8_t size = type;
    if (type == STRING) {
        size = popByte();
    }

    // check for free space
    uint8_t addr = checkMemoryTable(size);
    if (addr < 0) {
        Serial.println(F("Error: not enough space in the RAM."));
        return;
    }

    // create new entry in memory table
    Variable var = {name, type, size, addr, proc_id};

    // write bytes to memory
    for (int a = (addr + size); a > addr; a--) {
        memory[a] = popByte();
    }
    variables[no_of_vars++] = var;
}

/**
 * Search for a variable in memory and push it on the stack.
 * 
 * @param name name (1 byte) of the variabele.
 * @param proc_id the process id of the process this variable belongs to.
 */
void getVar(char name, int proc_id)
{

}

/**
 * Clear a variable from the memory table for a process.
 * 
 * @param name name (1 byte) of the variable.
 * @param proc_id the process if of the process this variable belongs to.
 */
void clearVar(char name, int proc_id)
{

}

/**
 * Clear all variabeles for a process.
 * 
 * @param proc_id the process to remove all variables from.
 */
void clearAllVars(int proc_id)
{

}