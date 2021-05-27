/*
 *
 * ArduinOS - CLI header file
 * include/cli.h
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
#define BACKSPACE           '\x08'
#define SPACE_CHAR          ' '
#define SPACE_STR           " "
         
// Function definitions
void argumentParser();
void help(CommandArgs args);

#endif