/*
    Source file for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 21-05-2021.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "filesystem.h"

EERef no_of_files = EEPROM[NO_OF_FILES_PTR] = 0;

void writeFATEntry(File file) {
    // write a file to the EEPROM.
    EEPROM.put(FAT_START_PTR + (no_of_files * sizeof(File)), file);
}

int findFATEntry(const char *name) {
    // find an entry on the EEPROM by name.
    if (no_of_files == 0) return -1;

    int eof_address = FAT_START_PTR;
    for (int entry = 0; entry < no_of_files; entry++) {
        File file = EEPROM.get(eof_address, file);
        if (strcmp(file.name, name) == 0) {
            return entry;
        }
        eof_address += sizeof(File);
    }
    return -1;
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
        else return FAT_START_PTR + sizeof(File);
    }
    
    int eof_address = FAT_START_PTR;
    File ee_files[(int)no_of_files];

    // FAT entries available, get data
    for (int entry = 0; entry < no_of_files; entry++) {
        EEPROM.get(eof_address, ee_files[entry]);
        eof_address += sizeof(File);
    }

    // check for free space between the first file & end of FAT
    if (ee_files[0].addr - eof_address > size) {
        return eof_address;
    }

    // sort multiple files based on start address
    if (no_of_files > 1) {
        int temp = 0;
        for (int index = 0; index < no_of_files; index++) {
            for (int next = index + 1; next < no_of_files; index++) {
                if (ee_files[index].addr > ee_files[next].addr) {
                    temp = ee_files[index].addr;
                    ee_files[index].addr= ee_files[next].addr;
                    ee_files[next].addr = temp;
                }
            }
        }
        // check for free space between all files
        for (int entry = 0; entry < no_of_files; entry++) {
            if ((ee_files[entry + 1].addr - (ee_files[entry].addr + ee_files[entry].size)) > size) {
                return ee_files[entry].addr + ee_files[entry].size;
            }
        }
    }
    
    // space till end of FAT
    if (((ee_files[0].addr + ee_files[0].size) - (int)EEPROM.length()) > size) {
        return ee_files[0].addr + ee_files[0].size;
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
    int size = atoi(argv->arg[1]);
    char *name = strdup(argv->arg[0]);
    //File file = {0};
    //strcpy(file.name, argv->arg[0]);
    // Data
    char *data = strdup(argv->arg[2]);

    Serial.println(name);
    Serial.println(size);
    Serial.println(data);

    // check if filename exists

    // free allocated memory by strdup
    free(name);
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
    Serial.println(no_of_files);
}

void freespace(CommandArgs *argv) {
    // print the amount of free space left on the EEPROM.
    int free_space = EEPROM.length();

    Serial.println(free_space);
}