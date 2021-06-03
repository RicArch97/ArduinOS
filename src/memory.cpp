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

// Sort memory table based on the begin address of variable data.
static void sortMemoryTable()
{
    for (int c = 0; c < no_of_vars; c++) {
        for (int n = c + 1; n < no_of_vars; n++) {
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
    // first checks
    if (no_of_vars == 0) {
        if (size > MEM_SIZE) {
            Serial.print(F("Error: cannot save a string with size: "));
            Serial.print(size);
            Serial.print(F("Max size is: "));
            Serial.println(MEM_SIZE);
            return -1;
        }
        else return 0;
    }
        
    // sort memory table based on begin address
    if (no_of_vars > 1)
        sortMemoryTable();

    // check for free space between start and first var
    if (variables[0].addr >= size)
        return 0;

    // check for free space between vars
    for (int e = 0; e < no_of_vars; e++) {
        if ((variables[e + 1].addr - (variables[e].addr + variables[e].size)) >= size) {
            return variables[e].addr + variables[e].size;
        }
    }

    // check between last var and end of memory
    if (MEM_SIZE - (variables[no_of_vars - 1].addr + variables[no_of_vars - 1].addr) >= size)
        return variables[no_of_vars - 1].addr + variables[no_of_vars - 1].size;

    Serial.println(F("Error: not enough space left in the RAM."));
    return -1;
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
    for (int e = 0; e < no_of_vars; e++) {
        if (name == variables[e].name && proc_id == variables[e].proc_id) {
            // remove item and shift remaining items
            memmove(variables + e, variables + e + 1, ((--no_of_vars - e)) * sizeof(Variable));
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
    if (addr < 0) 
        return;

    // create new entry in memory table
    Variable var = {name, type, size, addr, proc_id};

    // write bytes to memory
    for (int a = addr; a < (addr + size); a++) {
        memory[a] = popByte();
    }
    variables[no_of_vars++] = var;

    Serial.print(F("Sucessfully wrote variable "));
    Serial.print(name);
    Serial.println(F(" to memory."));
}

/**
 * Search for a variable in memory and push it on the stack.
 * 
 * @param name name (1 byte) of the variabele.
 * @param proc_id the process id of the process this variable belongs to.
 */
void getVar(char name, int proc_id)
{
    // check if var exists
    for (int e = 0; e < no_of_vars; e++) {
        if (name == variables[e].name && proc_id == variables[e].proc_id) {
            // push var data
            for (int a = variables[e].addr; a < (variables[e].addr + variables[e].size); a++) {
                pushByte(a);
            }
            // check if string, if so push size
            if (variables[e].type == STRING)
                pushByte(variables[e].size);
            // push the var type
            pushByte(variables[e].type);
            return;
        }
    }

    Serial.print(F("Error: variable with name "));
    Serial.print(name);
    Serial.println(F(" not found in memory table."));
}

/**
 * Clear a variable from the memory table for a process.
 * 
 * @param name name (1 byte) of the variable.
 * @param proc_id the process if of the process this variable belongs to.
 */
void clearVar(char name, int proc_id)
{
    for (int e = 0; e < no_of_vars; e++) {
        if (name == variables[e].name && proc_id == variables[e].proc_id) {
            // remove item and shift remaining items
            memmove(variables + e, variables + e + 1, ((--no_of_vars - e)) * sizeof(Variable));
        }
    }
}

/**
 * Clear all variabeles for a process.
 * 
 * @param proc_id the process to remove all variables from.
 */
void clearAllVars(int proc_id)
{
    for (int e = 0; e < no_of_vars; e++) {
        if (proc_id == variables[e].proc_id) {
            // remove item and shift remaining items
            memmove(variables + e, variables + e + 1, ((--no_of_vars - e)) * sizeof(Variable));
        }
    }
}

// Print the memory table entries. debug use only.
void debugPrintMemoryTable() 
{
    for (int i = 0; i < no_of_vars; i++) {
        Serial.print(i);
        Serial.print(F(": {n: "));
        Serial.print(variables[i].name);
        Serial.print(F(", t: "));
        Serial.print(variables[i].type);
        Serial.print(F(", s: "));
        Serial.print(variables[i].size);
        Serial.print(F(", a: "));
        Serial.print(variables[i].addr);
        Serial.print(F(", p: "));
        Serial.print(variables[i].proc_id);
        Serial.println(F("}"));
    }
}

// Print the data in the memory. debug use only.
void debugPrintMemory() 
{
    for (unsigned int i = 0; i < (sizeof(memory) / sizeof(uint8_t)); i++) {
        Serial.print(memory[i]);
    }
    Serial.println();
}