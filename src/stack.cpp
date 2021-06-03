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

uint8_t stack[STACKSIZE];
uint8_t sp = 0;

// Push a byte (char) to the stack.
void pushByte(uint8_t b) {
    stack[sp++] = b;
}

// Push an int to the stack by converting it to 2 bytes.
void pushInt(int i) 
{
    unsigned b;
    memcpy(&b, &i, sizeof(b));
    stack[sp++] = (b >> 8) & 0xFF;
    stack[sp++] = b & 0xFF;
    stack[sp++] = INT;
}

// Push a float to the stack by converting it to 4 bytes.
void pushFloat(float f)
{
    unsigned b;
    memcpy(&b, &f, sizeof(b));
    stack[sp++] = (b >> 24) & 0xFF;
    stack[sp++] = (b >> 16) & 0xFF;
    stack[sp++] = (b >> 8) & 0xFF;
    stack[sp++] = b & 0xFF;
    stack[sp++] = FLOAT;
}

// Push a string to the stack.
void pushString(const char *s)
{
    for (int i = 0; i < strlen(s); i++) {
        stack[sp++] = s[i];
    }
    stack[sp++] = '\0';
    stack[sp++] = strlen(s) + 1;
    stack[sp++] = STRING;
}

// Pop a byte (char) from the stack.
uint8_t popByte() {
    return stack[--sp];
}