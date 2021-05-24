/*
    Source file for the processes of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#include <Arduino.h>
#include "common.h"

void run(CommandArgs argv) {
    Serial.println("run called");
}

void list(CommandArgs argv) {
    Serial.println("list called");
}

void suspend(CommandArgs argv) {
    Serial.println("suspend called");
}

void resume(CommandArgs argv) {
    Serial.println("resume called");
}

void kill(CommandArgs argv) {
    Serial.println("kill called");
}