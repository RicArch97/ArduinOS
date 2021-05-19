/*
    Header for the CLI of ArduinOS.

    Created by Ricardo Steijn on 12-05-2021.
    Last update on 17-05-2021.
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

// Stub
#define STUB                "\n"                                                                    \
                            "ArduinOS 0.1, A Unix like operating system for Arduino.\n"             \
                            "Usage: command [options]\n"                                            \
                            "\n"                                                                    \
                            "Commands:\n"                                                           \
                            "store\t\t<file> <size> <data>\tStore a file in the filesystem.\n"      \
                            "retrieve\t<file>\t\t\tRequest a file from the filesystem.\n"           \
                            "erase\t\t<file>\t\t\tErase a file.\n"                                  \
                            "files\t\t\t\t\tList all files in the filesystem.\n"                    \
                            "freespace\t\t\t\tShow the amount of free space in the filesystem.\n"   \
                            "run\t\t<file>\t\t\tRun a program.\n"                                   \
                            "list\t\t\t\t\tShow a list with all processes.\n"                       \
                            "suspend\t\t<id>\t\t\tSuspend a process.\n"                             \
                            "resume\t\t<id>\t\t\tResume a process.\n"                               \
                            "kill\t\t<id>\t\t\tKill a process."                                     \
                            "\n"

// Function definitions
void argumentParser();
void help(CommandArgs args);

#endif