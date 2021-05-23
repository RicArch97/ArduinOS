/*
    Source file for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 21-05-2021.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "filesystem.h"

EERef no_of_files = EEPROM[NOF_PTR] = 0;

void writeFATEntry(File file) {
    // write a file to the EEPROM.
    EEPROM.put(FST_PTR + (no_of_files * sizeof(File)), file);
}

int findFATEntry(const char *name) {
    // find an entry on the EEPROM by name.
    if (no_of_files == 0) return -1;

    int eof_address = FST_PTR;
    for (int entry = 0; entry < no_of_files; entry++) {
        File file = EEPROM.get(eof_address, file);
        if (strcmp(file.name, name) == 0) {
            return entry;
        }
        eof_address += sizeof(File);
    }
    return -1;
}

int sortFAT() {
    // sort multiple files based on start address
    int eof_addr = FST_PTR;
    
    // loop through all structs on the EEPROM
    if (no_of_files > 1) {
        File temp = {0};
        for (int c = eof_addr; c < (FST_PTR + (no_of_files * sizeof(File))); c += sizeof(File)) {
            for (int n = c + sizeof(File); n < (FST_PTR + (no_of_files * sizeof(File))); n += sizeof(File)) {
                // compare current file with next file
                File c_file = (File)EEPROM.get(c, c_file);
                File n_file = (File)EEPROM.get(n, n_file);

                if (c_file.addr > n_file.addr) {
                    temp = c_file;
                    EEPROM.put(c, n_file);
                    EEPROM.put(n, temp);
                }
            }
            eof_addr += sizeof(File);
        }   
    }   
    return eof_addr;
}

int checkFATSpace(int size) {
    // check for free space on the EEPROM.
    // first check if we need to check at all
    if (no_of_files == AMOUNT_OF_FILES) return -1;
    if (no_of_files == 0) {
        if (size > (int)(EEPROM.length() - sizeof(File))) {
            Serial.print(F("Cannot save a file with size: "));
            Serial.print(size);
            Serial.print(F("Max size is: "));
            Serial.println(EEPROM.length() - sizeof(File));
            return -1;
        }
        else return FST_PTR;
    }

    // sort FAT based on begin address, get end of FAT pointer
    int eof_address = sortFAT();

    // check for free space between the first file & end of FAT
    File first_file = (File)EEPROM.get(FST_PTR, first_file);
    if ((first_file.addr - eof_address) > size) {
        return eof_address;
    }

    // look for free space between all the files
    for (int e = FST_PTR; e < (FST_PTR + (no_of_files * sizeof(File))); e += sizeof(File)) {
        File c_file = EEPROM.get(e, c_file);
        File n_file = EEPROM.get((e + sizeof(File)), n_file);

        if ((n_file.addr - (c_file.addr + c_file.size)) > size) {
            return FST_PTR + (c_file.addr + c_file.size);
        }
    }
    
    File last_file = EEPROM.get((FST_PTR + ((no_of_files * sizeof(File)) - sizeof(File))), last_file);
    // space till end of FAT
    if (((int)EEPROM.length() - (last_file.addr + last_file.size)) > size) {
        return FST_PTR + (last_file.addr + last_file.size);
    }
    else {
        Serial.println(F("Not enough space left in the filesystem."));
        return -1;
    }

    return -1; 
}

void store(CommandArgs *argv) {
    // store a file in the filesystem.
    for (int i = 0; i < MAX_ARG_AMOUNT; i++) {
        if (strlen(argv->arg[i]) == 0) {
            Serial.println(F("Not enough arguments provided."));
            return;
        }
    }
    // Create file using name and size.
    int size = atoi(argv->arg[2]);
    File file = {0};
    strcpy(file.name, argv->arg[0]);
    // Data
    char *data = strdup(argv->arg[2]);

    // check if filename exists

    // free allocated memory by strdup
    free(data);
}

void retrieve(CommandArgs *argv) {
    // retrieve a file from the filesystem and print the data.
    char *file_name;
    if (argv->arg[0] != NULL) {
        file_name = strdup(argv->arg[0]);
    }

    free(file_name);
}

void erase(CommandArgs *argv) {
    // erase a file from the filesystem.
    char *file_name;
    if (argv->arg[0] != NULL) {
        file_name = strdup(argv->arg[0]);
    }

    free(file_name);
}

void files(CommandArgs *argv) {
    // list all files in the filesystem.
    for (int i = 0; i < EEPROM.length(); i++) {
        Serial.println(EEPROM.read(i));
    }
}

void freespace(CommandArgs *argv) {
    // print the amount of free space left on the EEPROM.
    int free_space = EEPROM.length();

    Serial.println(free_space);
}