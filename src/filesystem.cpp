/*
 *
 * ArduinOS - Filesystem source file
 * src/filesystem.cpp
 *
 * Copyright (C) 2021 Ricardo Steijn <0955903@hr.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "filesystem.h"
#include "cli.h"

EERef no_of_files = EEPROM[NOF_PTR];

// Initialize `no_of_files` to zero if EEPROM empty, otherwise use existing value on EEPROM.
void initFileSystem() 
{
    if (EEPROM.read(NOF_PTR) == 0xFF) {
        no_of_files = 0;
    }
}

/**
 * Store a file in the ArduinOS filesystem.
 * 
 * @param name name of the file.
 * @return begin address pointer of the FAT entry.
 */
int findFATEntry(const char *name) 
{
    // find an entry on the EEPROM by name.
    // returns: address pointer of the FAT entry.
    if (no_of_files == 0) return -1;

    int eof_last_file_ptr = FST_PTR + ((int)no_of_files * sizeof(File));
    // compare the name of every file struct to the arg
    for (int e = FST_PTR; e < eof_last_file_ptr; e += sizeof(File)) {
        File file = (File)EEPROM.get(e, file);
        if (strcmp(file.name, name) == 0) {
            return e;
        }
    }
    return -1;
}

/**
 * Read a FAT entry from the EEPROM.
 * 
 * @param addr begin address pointer of the FAT entry on the EEPROM.
 * @return File struct representing a FAT entry.
 */
File readFATEntry(int addr)
{
    File file = EEPROM.get(addr, file);
    return file;
}

/**
 * Read one byte of data from the EEPROM at the program counter.
 * 
 * @param pc program counter of a process.
 * @return one byte (uint8_t) of data at the pc location.
 */
uint8_t readPcByte(int pc)
{
    return EEPROM.read(pc);
}

// Write a file to the EEPROM.
static void writeFATEntry(File file) 
{
    EEPROM.put(FST_PTR + ((int)no_of_files * sizeof(File)), file);
    // update number of files in EEPROM
    no_of_files++;
}

// Write data to the referenced address in the FAT.
static void writeData(int addr, int size, char *data) 
{
    int str_index = 0;
    for (int b = addr; b < (addr + size); b++) {
        EEPROM.write(b, data[str_index]);
        str_index++;
    }
}

// Sort the FAT based on the address of the filedata.
static void sortFAT() 
{
    int eof_last_file_ptr = FST_PTR + ((int)no_of_files * sizeof(File));
    // compare current file with next file
    for (int c = FST_PTR; c < eof_last_file_ptr; c += sizeof(File)) {
        for (int n = c + sizeof(File); n < eof_last_file_ptr; n += sizeof(File)) {
            File c_file = (File)EEPROM.get(c, c_file);
            File n_file = (File)EEPROM.get(n, n_file);

            if (c_file.addr > n_file.addr) {
                File temp = c_file;
                EEPROM.put(c, n_file);
                EEPROM.put(n, temp);
            }
        }
    }    
}

/**
 * Check the available space in the filesystem.
 * 
 * @param size size of the file that needs to be allocated.
 * @return begin pointer on the EEPROM where the file can be written when `size` is a positive integer, 
 *      or the max size that can be used by files in the filesystem when `size` is a negetive integer.
 */
static int checkFileSystemSpace(int size) 
{
    int max_free_space = 0;

    // first check if we need to check at all
    if ((int)no_of_files == AMOUNT_OF_FILES) {
        if (size < 0) return max_free_space;
        Serial.println(F("Error: file limit reached."));
        return -1;
    }

    // calculate end of FAT pointer based on amount of files
    int eof_address = FST_PTR + (AMOUNT_OF_FILES * sizeof(File));

    // the amount of space for data is limited
    if ((int)no_of_files == 0) {
        if (size < 0) {
            return (int)EEPROM.length() - eof_address;
        }
        else if (size > ((int)EEPROM.length() - eof_address)) {
            Serial.print(F("Error: cannot save a file with size: "));
            Serial.print(size);
            Serial.print(F("Max size is: "));
            Serial.println((int)EEPROM.length() - eof_address);
            return -1;
        }
        else return eof_address;
    }

    // sort FAT based on begin address
    if ((int)no_of_files > 1) 
        sortFAT();

    // check for free space between the first file & end of FAT
    File first_file = (File)EEPROM.get(FST_PTR, first_file);
    if (size < 0) max_free_space = first_file.addr - eof_address;
    else if ((first_file.addr - eof_address) >= size) {
        return eof_address;
    }

    int eof_last_file_ptr = FST_PTR + ((int)no_of_files * sizeof(File));

    // look for free space between all the files
    for (int a = FST_PTR; a < eof_last_file_ptr; a += sizeof(File)) {
        File c_file = (File)EEPROM.get(a, c_file);
        File n_file = (File)EEPROM.get((a + sizeof(File)), n_file);

        if (size < 0) {
            if ((n_file.addr - (c_file.addr + c_file.size)) > max_free_space) {
                max_free_space = n_file.addr - (c_file.addr + c_file.size);
            }
            else continue;
        }
        else if ((n_file.addr - (c_file.addr + c_file.size)) >= size) {
            return c_file.addr + c_file.size;
        }
    }
    
    File last_file = EEPROM.get((eof_last_file_ptr - sizeof(File)), last_file);
    int remaining_file_space = (int)EEPROM.length() - (last_file.addr + last_file.size);
    // space till end of EEPROM
    if (size < 0) {
        if (remaining_file_space > max_free_space) {
            max_free_space = remaining_file_space;
        }
        return max_free_space;
    }
    else if (remaining_file_space >= size) {
        return last_file.addr + last_file.size;
    }
   
    Serial.println(F("Error: not enough space left in the filesystem."));
    return -1;
}

/**
 * Store a file in the ArduinOS filesystem.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void store(CommandArgs argv) 
{
    for (int i = 0; i < MAX_ARG_AMOUNT; i++) {
        if (strlen(argv.arg[i]) == 0) {
            Serial.println(F("Error: Not enough arguments provided."));
            return;
        }
    }
    // check that the size is a positive number
    int size = atoi(argv.arg[1]);
    if (size <= 0) {
        Serial.println(F("Error: the \"size\" argument should be a postitive number."));
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
    int blk_ptr = checkFileSystemSpace(size);
    if (blk_ptr < FST_PTR) {
        free(name);
        return;
    }

    // create a file in the filesystem
    File file = {0};
    strcpy(file.name, name);
    file.addr = blk_ptr;
    file.size = size;
    writeFATEntry(file);

    // write the file data to the drive
    char *data = (char*)malloc(size);
    Serial.readBytes(data, size);
    writeData(blk_ptr, size, data);

    Serial.print(F("File \""));
    Serial.print(file.name);
    Serial.println(F("\" stored successfully."));
    free(name);
    free(data);
}

/**
 * Retrieve a file from the ArduinOS filesystem and print the data.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void retrieve(CommandArgs argv) 
{
    char *file_name;
    if (strlen(argv.arg[0]) > 0) {
        file_name = strdup(argv.arg[0]);
    }
    else {
        Serial.println(F("Error: the filename argument is required."));
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
    Serial.print(F("Data in file \""));
    Serial.print(file_name);
    Serial.print(F("\": "));
    for (int i = file.addr; i < (file.addr + file.size); i++) {
        // 255 means empty in the EEPROM, also empty character in ASCII table
        if ((int)EEPROM.read(i) != 0xFF) {
            Serial.print((char)EEPROM.read(i));
        }
    }
    Serial.println();

    free(file_name);
}

/**
 * Erase a file in the ArduinOS filesystem.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void erase(CommandArgs argv) 
{
    char *file_name;
    if (strlen(argv.arg[0]) > 0) {
        file_name = strdup(argv.arg[0]);
    }
    else {
        Serial.println(F("Error: the filename argument is required."));
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

    // remove the file data by writing original EEPROM values
    File file = (File)EEPROM.get(f_addr, file);
    for (int b = file.addr; b < (file.addr + file.size); b++) {
        EEPROM.write(b, 0xFF);
    }

    int eof_last_file_ptr = FST_PTR + ((int)no_of_files * sizeof(File));
    int last_file_ptr = FST_PTR + (((int)no_of_files - 1) * sizeof(File));
    // move existing extries in the FAT
    for (int a = f_addr; a < last_file_ptr; a += sizeof(File)) {
        File n_file = (File)EEPROM.get((a + sizeof(File)), n_file);
        EEPROM.put(a, n_file);
    }

    // remove the remaining entry by rewriting the original EEPROM values
    for (int a = last_file_ptr; a < eof_last_file_ptr; a++) {
        EEPROM.write(a, 0xFF);
    }
    // update number of files in EEPROM
    no_of_files--;

    Serial.print(F("File \""));
    Serial.print(file.name);
    Serial.println(F("\" removed successfully."));
    free(file_name);
}

/**
 * Print the files and their respective sizes within the filesystem of ArduinOS.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void files(CommandArgs argv) 
{
    if ((int)no_of_files == 0) {
        Serial.println(F("No files in the filesystem."));
        return;
    }
    int eof_last_file_ptr = FST_PTR + ((int)no_of_files * sizeof(File));
    // loop through all the files in list the names.
    for (int e = FST_PTR; e < eof_last_file_ptr; e += sizeof(File)) {
        File file = (File)EEPROM.get(e, file);
        Serial.print(file.name);
        Serial.print(F(", "));
        Serial.print(file.size);
        Serial.println(F(" bytes."));
    }
}

/**
 * Print the size of the largest available space in the ArduinOS filesystem.
 * 
 * @param argv CommandArgs struct with string arguments.
 */
void freespace(CommandArgs argv) 
{
    Serial.print(F("Free space available in filesystem: "));
    Serial.print(checkFileSystemSpace(-1));
    Serial.println(F(" bytes."));
}

// Reset all values on EEPROM back to original. Debug use only.
void debugResetEEPROM() 
{
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0xFF);
    }
}

// Print all bytes on the EEPROM. Debug use only.
void debugPrintEEPROM()
{
    int counter = 0;
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
        if (counter < 30) {
            Serial.print(EEPROM.read(i));
        }
        else {
            Serial.println();
            counter = 0;
        }
    }
    Serial.println();
}