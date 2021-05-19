/*
  Main file for ArduinOS.

  Created by Ricardo Steijn on 12-05-2012.
  Last update on 19-05-2021.
*/

#include <Arduino.h>
#include "cli.h"

void setup() {
  Serial.begin(9600);

  Serial.println("Welcome to ArduinOS on tty1.");
  Serial.println("Type \"help\" to see a list of commands.");
}

void loop() {
  argumentParser();
}