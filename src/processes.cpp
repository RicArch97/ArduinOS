/*
    Source file for the processes of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#include <Arduino.h>
#include "common.h"

void suspend(CommandArgs args) {
    Serial.println("suspend called");
}

void resume(CommandArgs args) {
    Serial.println("resume called");
}

void kill(CommandArgs args) {
    Serial.println("kill called");
}

void id() {
    Serial.println("id argument used");
}