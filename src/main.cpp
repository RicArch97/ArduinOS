/*
 *
 * ArduinOS - Main file, which runs a setup and then an infinite loop.
 * src/main.cpp
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
#include "cli.h"
#include "filesystem.h"
#include "processes.h"

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(-1);

  Serial.println(F("ArduinOS 0.1 ready. Type \"help\" to see a list of commands."));

  initFileSystem();
}

void loop() {
  // non-blocking argument parser
  argumentParser();
  // execute instruction for all processes
  runProcesses();
}