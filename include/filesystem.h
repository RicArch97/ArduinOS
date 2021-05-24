/*
    Header for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 21-05-2021.
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"

#define AMOUNT_OF_FILES 10
#define FILENAME_SIZE   12
#define NOF_PTR         0x00
#define FST_PTR         0x01

typedef struct {
    char name[FILENAME_SIZE];
    int addr;
    int size;
} File;

int findFATEntry();
void writeFATEntry(File file);
void writeData(int addr, char *data);
int sortFAT();
int checkFATSpace();

void store(CommandArgs *argv);
void retrieve(CommandArgs *argv);
void erase(CommandArgs *argv);
void files(CommandArgs *argv);
void freespace(CommandArgs *argv);

#endif