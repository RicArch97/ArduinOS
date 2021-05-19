/*
    Source file for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"

void store(CommandArgs args) {
    Serial.println("store called");
}

void retrieve(CommandArgs args) {
    Serial.println("retrieve called");
}

void erase(CommandArgs args) {
    Serial.println("erase called");
}

void files(CommandArgs args) {
    Serial.println("files called");
}

void freespace(CommandArgs args) {
    Serial.println("freespace called");
}

void run(CommandArgs args) {
    Serial.println("run called");
}

void list(CommandArgs args) {
    Serial.println("list called");
}

void file() {
    Serial.println("file argument used");
}

void size() {
    Serial.println("size argument used");
}

void data() {
    Serial.println("data argument used");
}