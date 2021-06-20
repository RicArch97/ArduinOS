/*
 *
 * ArduinOS - Stack source file
 * src/stack.cpp
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
#include "stack.h"
#include "instruction_set.h"
#include "processes.h"


/**
 * Push a byte to the stack. This function does not focus on a specific type.
 * 
 * @param id process id of the process.
 */
void pushByte(uint8_t b, int id) 
{
    Process *p = getProcessById(id);
    if (p != NULL)
        p->stack[p->sp++] = b;
}

/**
 * Push a char to the stack.
 * 
 * @param id process id of the process.
 */
void pushChar(char c, int id)
{
    pushByte(c, id);
    pushByte(CHAR, id);
}

/**
 * Push an int to the stack by converting it to 2 bytes.
 * 
 * @param id process id of the process.
 */
void pushInt(int i, int id) 
{
    uint16_t b;
    memcpy(&b, &i, sizeof(b));
    pushByte(((b >> 8) & 0xFF), id);
    pushByte((b & 0xFF), id);
    pushByte(INT, id);
}

/**
 * Push a float to the stack by converting it to 4 bytes.
 * 
 * @param id process id of the process.
 */
void pushFloat(float f, int id)
{
    uint32_t b;
    memcpy(&b, &f, sizeof(b));
    pushByte(((b >> 24) & 0xFF), id);
    pushByte(((b >> 16) & 0xFF), id);
    pushByte(((b >> 8) & 0xFF), id);
    pushByte((b & 0xFF), id);
    pushByte(FLOAT, id);
}

/**
 * Push a string to the stack.
 * 
 * @param id process id of the process.
 */
void pushString(const char *s, int id)
{
    for (size_t i = 0; i < strlen(s); i++) {
        pushByte(s[i], id);
    }
    pushByte('\0', id);
    pushByte((strlen(s) + 1), id);
    pushByte(STRING, id);
}

/**
 * Pop one byte from the stack. This function does not focus on a specific type.
 * 
 * @param id process id of the process.
 * @return next byte from the stack, or 0 when there's nothing on the stack.
 */
uint8_t popByte(int id) 
{
    Process *p = getProcessById(id);
    if (p == NULL || p->sp == 0)
        return 0;

    return p->stack[--p->sp];
}

/**
 * Pop an int from the stack.
 * 
 * @param id process id of the process.
 * @return char from the stack, or (char)0 on failure.
 */
char popChar(int id)
{
    if (popByte(id) != CHAR)
        return '\0';

    return (char)popByte(id);
}

/**
 * Pop an int from the stack.
 * 
 * @param id process id of the process.
 * @return int value from the stack or -1 on failure.
 */
int popInt(int id) 
{
    if (popByte(id) != INT)
        return -1;

    int i;
    uint8_t b[] = {popByte(id), popByte(id)};
    memcpy(&i, &b, sizeof(i));
    return i;
}

/**
 * Pop a float from the stack.
 * 
 * @param id process id of the process.
 * @return float value from the stack or (float)-1 on failure.
 */
float popFloat(int id) 
{
    if (popByte(id) != FLOAT)
        return (float)-1.0;
    
    float f;
    uint8_t b[] = {popByte(id), popByte(id), popByte(id), popByte(id)};
    memcpy(&f, &b, sizeof(f));
    return f;
}

/**
 * Pop a string from the stack.
 * 
 * @param id process id of the process.
 * @return pointer to the string returned from the stack. (char)0 on failure.
 */
const char *popString(int id)
{
    if (popByte(id) != STRING)
        return "";

    int size = popByte(id);

    char *s = (char*)malloc(size);
    for (int i = 0; i < size; i++) {
        s[i] = popByte(id);
    }
    free(s);

    return s;
}