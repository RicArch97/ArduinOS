/*
    Source file for the filesystem of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 21-05-2021.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "filesystem.h"

EERef no_of_files = getNoOfFiles();

int getNoOfFiles() {
    // initiate to zero or get current value.
    if (EEPROM.read(NOF_PTR) == 0xFF) {
        return EEPROM[NOF_PTR] = 0;
    }
    return EEPROM[NOF_PTR];
}

int findFATEntry(const char *name) {
    // find an entry on the EEPROM by name.
    if (no_of_files == 0) return -1;

    for (unsigned int e = FST_PTR; e < (FST_PTR + (no_of_files * sizeof(File))); e += sizeof(File)) {
        File file = (File)EEPROM.get(e, file);
        if (strcmp(file.name, name) == 0) {
            return e;
        }
    }
    return -1;
}

void writeFATEntry(File file) {
    // write a file to the EEPROM.
    EEPROM.put(FST_PTR + (no_of_files * sizeof(File)), file);
    no_of_files++;
}

void writeData(int addr, int size, char *data) {
    // write data to the referenced address in the FAT.
    int str_index = 0;
    for (int b = addr; b < (addr + size); b++) {
        if (data[str_index] != '\0') {
            EEPROM.write(b, data[str_index]);
        }
        str_index++;
    }
}

void sortFAT() {
    // loop through all structs on the EEPROM
    File temp = {0};
    for (unsigned int c = FST_PTR; c < (FST_PTR + (no_of_files * sizeof(File))); c += sizeof(File)) {
        for (unsigned int n = c + sizeof(File); n < (FST_PTR + (no_of_files * sizeof(File))); n += sizeof(File)) {
            // compare current file with next file
            File c_file = (File)EEPROM.get(c, c_file);
            File n_file = (File)EEPROM.get(n, n_file);

            if (c_file.addr > n_file.addr) {
                temp = c_file;
                EEPROM.put(c, n_file);
                EEPROM.put(n, temp);
            }
        }
    }    
}

int checkFATSpace(int size) {
    // check for free space on the EEPROM.
    // first check if we need to check at all
    if (no_of_files == AMOUNT_OF_FILES) return -1;

    // calculate end of FAT pointer based on amount of files
    int eof_address = FST_PTR + (AMOUNT_OF_FILES * sizeof(File));

    // the amount of space for data is limited
    if (no_of_files == 0) {
        if (size > ((int)EEPROM.length() - eof_address)) {
            Serial.print(F("Error: cannot save a file with size: "));
            Serial.print(size);
            Serial.print(F("Max size is: "));
            Serial.println((int)EEPROM.length() - eof_address);
            return -1;
        }
        else return eof_address;
    }

    // sort FAT based on begin address
    if (no_of_files > 1) {
        sortFAT();
    }

    // check for free space between the first file & end of FAT
    File first_file = (File)EEPROM.get(FST_PTR, first_file);
    if ((first_file.addr - eof_address) > size) {
        return eof_address;
    }

    // look for free space between all the files
    for (unsigned int e = FST_PTR; e < (FST_PTR + (no_of_files * sizeof(File))); e += sizeof(File)) {
        File c_file = (File)EEPROM.get(e, c_file);
        File n_file = (File)EEPROM.get((e + sizeof(File)), n_file);

        if ((n_file.addr - (c_file.addr + c_file.size)) > size) {
            return c_file.addr + c_file.size;
        }
    }
    
    File last_file = EEPROM.get((FST_PTR + ((no_of_files * sizeof(File)) - sizeof(File))), last_file);
    // space till end of FAT
    if (((int)EEPROM.length() - (last_file.addr + last_file.size)) > size) {
        Serial.println(last_file.addr + last_file.size);
        return last_file.addr + last_file.size;
    }
    else {
        Serial.println(F("Error: not enough space left in the filesystem."));
        return -1;
    }

    return -1; 
}

void store(CommandArgs argv) {
    // store a file in the filesystem.
    for (int i = 0; i < MAX_ARG_AMOUNT; i++) {
        if (strlen(argv.arg[i]) == 0) {
            Serial.println(F("Error: Not enough arguments provided."));
            return;
        }
    }
    // check that the size is a number
    int size = atoi(argv.arg[1]);
    if (size <= 0) {
        Serial.println(F("Error: the \"size\" argument should be a number."));
        return;
    }

    char *name = strdup(argv.arg[0]);
    // check if filename exists
    if (findFATEntry(name) > 0) {
        Serial.print(F("Error: file with name \""));
        Serial.print(name);
        Serial.println(F("\" already exists in the filesystem."));

        free(name);
        return;
    }

    // check for free space in the FAT & drive
    int blk_ptr = checkFATSpace(size);
    if (blk_ptr < FST_PTR) {
        free(name);
        return;
    }

    // create a file in the filesystem
    File file;
    strcpy(file.name, name);
    file.addr = blk_ptr;
    file.size = size;
    writeFATEntry(file);

    // write the file data to the drive
    char *data = strdup(argv.arg[2]);
    writeData(blk_ptr, size, data);

    free(name);
    free(data);
}

void retrieve(CommandArgs argv) {
    // retrieve a file from the filesystem and print the data.
    char *file_name;
    if (argv.arg[0] != NULL) {
        file_name = strdup(argv.arg[0]);
    }
    else {
        Serial.println(F("Error: the \"filename\" argument is required."));
        return;
    }

    // check if the file exists
    int f_addr = findFATEntry(file_name);
    if (f_addr < 0) {
        Serial.print(F("Error: no file with name \""));
        Serial.print(file_name);
        Serial.println(F("\" exists in the filesystem."));

        free(file_name);
        return;
    }

    // print the data
    File file = EEPROM.get(f_addr, file);
    Serial.print("Data in file \"");
    Serial.print(file_name);
    Serial.print("\": ");
    for (int i = file.addr; i < (file.addr + file.size); i++) {
        Serial.print((char)EEPROM.read(i));
    }
    Serial.println();

    free(file_name);
}

void erase(CommandArgs argv) {
    // erase a file from the filesystem.
    char *file_name;
    if (argv.arg[0] != NULL) {
        file_name = strdup(argv.arg[0]);
    }

    free(file_name);
}

void files(CommandArgs argv) {
    // list all files in the filesystem.
    int counter = 0;
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
        if (counter < 100) {
           Serial.print(EEPROM.read(i));
           counter++; 
        }
        if (counter == 100) {
            Serial.println();
            counter = 0;
        }
    }
}

void freespace(CommandArgs argv) {
    // print the amount of free space left on the EEPROM.
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0xFF);
    }
}