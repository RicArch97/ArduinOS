/*
    Source file for the CLI of ArduinOS.

    Created by Ricardo Steijn on 12-05-2021.
    Last update on 19-05-2021.
*/

#include <Arduino.h>
#include "common.h"
#include "cli.h"
#include "filesystem.h"
#include "processes.h"

typedef struct {
    char name[COMMAND_NAMESIZE];
    void (*func)(CommandArgs*);
} CommandType;

static char buffer[BUFSIZE];
static int char_count = 0;
static CommandArgs argv;

static const CommandType command[] = {
    {HELP, &help},
    {STORE, &store},
    {RETRIEVE, &retrieve},
    {ERASE, &erase},
    {FILES, &files},
    {FREESPACE, &freespace},
    {RUN, &run},
    {LIST, &list},
    {SUSPEND, &suspend},
    {RESUME, &resume},
    {KILL, &kill},
};

void argumentParser() {
    // Parse given CLI commands.

    if (Serial.available() > 0) {
        char c = Serial.read();

        if (c == CR || c == LF) {
            if (char_count > 0) {
                bool command_found = false;
                bool error_flag = false;

                for (unsigned int n = 0; n < (sizeof(command) / sizeof(CommandType)); n++) {
                    if (memchr(buffer, SPACE_CHAR, sizeof(buffer)) != NULL) {
                        char *arg;
                        char *tmp_buf = strdup(buffer);
                        int arg_index = -1;

                        // get string until token (space)
                        arg = strtok(tmp_buf, SPACE_STR);
                        while (arg != NULL) {
                            // command
                            if (arg_index < 0) {
                                if (strcmp(command[n].name, arg) == 0) {
                                    command_found = true;
                                    arg = strtok(NULL, SPACE_STR);
                                    arg_index++;
                                }
                                else break;
                            }
                            // args
                            if (arg_index < MAX_ARG_AMOUNT) {
                                if (strlen(arg) < ARG_NAMESIZE) {
                                    strcpy(argv.arg[arg_index], arg);
                                    arg = strtok(NULL, SPACE_STR);
                                    arg_index++;
                                }
                                else {
                                    Serial.println(F("Given argument too long."));
                                    error_flag = true;
                                    break;
                                }
                            }
                            else break;
                        }
                        // free memory allocated by strdup
                        free(tmp_buf);
                    }
                    else {
                        if (strcmp(command[n].name, buffer) == 0) {
                            command_found = true;
                        }
                    }
                    if (!error_flag && command_found) {
                        command[n].func(&argv);
                        break;
                    }
                }
                if (!error_flag && !command_found) {
                    // argument doesn't exist, show stub
                    Serial.print(F("Command not found: "));
                    Serial.println(buffer);
                    Serial.println(F("Type \"help\" to see a list of commands."));
                }
                // wipe CLI buffer
                memset(buffer, 0x00, sizeof(buffer));
                char_count = 0;
                // reset arg struct
                memset(&argv, 0, sizeof(CommandArgs));
            }
        }
        else {
            if (char_count < BUFSIZE) {
                buffer[char_count] = c;
                char_count++;
            }
            else {
                Serial.println(F("\nInput line too long."));
                memset(buffer, 0x00, sizeof(buffer));
                char_count = 0;
            }
        }
        delayMicroseconds(1024);
    }
}

void help(CommandArgs *argv) {
    Serial.println(F(
        "\n"
        "ArduinOS 0.1, A Unix like operating system for Arduino.\n"
        "\n"
        "Commands:\n"
        "store\t\t<file> <size> <data>\tStore a file in the filesystem.\n"
        "retrieve\t<file>\t\t\tRequest a file from the filesystem.\n"
        "erase\t\t<file>\t\t\tErase a file.\n"
        "files\t\t\t\t\tList all files in the filesystem.\n"
        "freespace\t\t\t\tShow the amount of free space in the filesystem.\n"
        "run\t\t<file>\t\t\tRun a program.\n"
        "list\t\t\t\t\tShow a list with all processes.\n"
        "suspend\t\t<id>\t\t\tSuspend a process.\n"
        "resume\t\t<id>\t\t\tResume a process.\n"
        "kill\t\t<id>\t\t\tKill a process."
        "\n"
    ));
}