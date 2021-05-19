/*
    Header for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"

void store(CommandArgs args);
void retrieve(CommandArgs args);
void erase(CommandArgs args);
void files(CommandArgs args);
void freespace(CommandArgs args);
void run(CommandArgs args);
void list(CommandArgs args);

void file();
void size();
void data();

#endif