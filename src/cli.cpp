/*
    Source file for the CLI of ArduinOS.

    Created by Ricardo Steijn on 12-05-2021.
    Last update on 17-05-2021.
*/

#include <Arduino.h>
#include "common.h"
#include "cli.h"
#include "filesystem.h"
#include "processes.h"

typedef struct {
    char name[COMMAND_NAMESIZE];
    void (*func)(CommandArgs);
} CommandType;

static char buffer[BUFSIZE];
static int charCount = 0;

static CommandType command[] = {
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
            if (charCount > 0) {
                bool commandFoundFlag = false;
                bool errorFlag = false;
                CommandArgs args;

                for (unsigned int n = 0; n < (sizeof(command) / sizeof(CommandType)); n++) {
                    if (memchr(buffer, SPACE_CHAR, sizeof(buffer)) != NULL) {
                        char *arg;
                        char *tmpBuf = strdup(buffer);
                        int argIndex = -1;

                        // get string until token (space)
                        arg = strtok(tmpBuf, SPACE_STR);
                        while (arg != NULL) {
                            // command
                            if (argIndex < 0) {
                                if (strcmp(command[n].name, arg) == 0) {
                                    commandFoundFlag = true;
                                    arg = strtok(NULL, SPACE_STR);
                                    argIndex++;
                                }
                                else break;
                            }
                            // args
                            if (argIndex < MAX_ARG_COUNT) {
                                if (strlen(arg) < ARG_NAMESIZE) {
                                    strcpy(args.args[argIndex], arg);
                                    arg = strtok(NULL, SPACE_STR);
                                    argIndex++;
                                }
                                else {
                                    Serial.println("\nGiven argument too long.");
                                    errorFlag = true;
                                    break;
                                }
                            }
                            else break;
                        }
                        // free memory allocated by strdup
                        free(tmpBuf);
                    }
                    else {
                        if (strcmp(command[n].name, buffer) == 0) {
                            commandFoundFlag = true;
                        }
                    }
                    if (!errorFlag && commandFoundFlag) {
                        command[n].func(args);
                        break;
                    }
                }
                if (!errorFlag && !commandFoundFlag) {
                    // argument doesn't exist, show stub
                    Serial.println("Given command not found. Type \"help\" to see a list of commands.");
                }
                // wipe CLI buffer
                memset(buffer, 0x00, sizeof(buffer));
                charCount = 0;
            }
        }
        else {
            if (charCount < BUFSIZE) {
                buffer[charCount] = c;
                charCount++;
            }
            else {
                Serial.println("\nInput line too long.");
                memset(buffer, 0x00, sizeof(buffer));
                charCount = 0;
            }
        }
    }
}

void help(CommandArgs args) {
    Serial.println(STUB);
}