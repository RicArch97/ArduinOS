/*
 *
 * ArduinOS - Stack header file
 * include/stack.h
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

#ifndef STACK_H
#define STACK_H

#include <Arduino.h>

#define STACKSIZE   32

void pushByte(uint8_t b, int id);
void pushChar(char c, int id);
void pushInt(int i, int id);
void pushFloat(float f, int id);
void pushString(const char *s, int id);

uint8_t popByte(int id);
char popChar(int id);
int popInt(int id);
float popFloat(int id);
char *popString(int id);

#endif