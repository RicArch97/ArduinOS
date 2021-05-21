/*
    Header for the CLI of ArduinOS.

    Created by Ricardo Steijn on 12-05-2021.
    Last update on 19-05-2021.
*/

#ifndef CLI_H
#define CLI_H

#include "common.h"

// Command buffer
#define BUFSIZE             128
#define COMMAND_NAMESIZE    12

// Command names
#define HELP                "help"
#define STORE               "store"
#define RETRIEVE            "retrieve"
#define ERASE               "erase"
#define FILES               "files"
#define FREESPACE           "freespace"
#define RUN                 "run"
#define LIST                "list"
#define SUSPEND             "suspend"
#define RESUME              "resume"
#define KILL                "kill"

// Tokens
#define CR                  '\r'
#define LF                  '\n'
#define SPACE_CHAR          ' '
#define SPACE_STR           " "

// Function definitions
void argumentParser();
void help(CommandArgs *args);

#endif