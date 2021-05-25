/*
  Main file for ArduinOS.

  Created by Ricardo Steijn on 12-05-2012.
  Last update on 19-05-2021.
*/

#include <Arduino.h>
#include "cli.h"
#include "filesystem.h"

void setup() {
  Serial.begin(9600);

  Serial.println(F("Welcome to ArduinOS on tty1."));
  Serial.println(F("Type \"help\" to see a list of commands."));

  initFileSystem();
}

void loop() {
  argumentParser();
}