/*
 *
 * ArduinOS - Tests for ArduinOS source file
 * src/test.cpp
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
 */


#include <Arduino.h>
#include "memory.h"
#include "instruction_set.h"
#include "stack.h"

// Test memory by pushing all possible datatypes and looking at the result.
void testMemory() 
{
    Serial.println();

    // test pushing char
    pushByte('a');
    pushByte(CHAR);
    setVar('x', 0);

    // test pushing integer
    pushByte(0);
    pushByte(12);
    pushByte(INT);
    setVar('y', 1);

    // test pushing string
    pushByte('H');
    pushByte('e');
    pushByte('l');
    pushByte('l');
    pushByte('o');
    pushByte(0);  // Terminating zero
    pushByte(6);  // String size
    pushByte(STRING);
    setVar('s', 2);

    // test changing int
    pushByte(0);
    pushByte(50);
    pushByte(INT);
    setVar('y', 1);

    // test another char
    pushByte('b');
    pushByte(CHAR);
    setVar('z', 0);

    debugPrintMemoryTable();
}