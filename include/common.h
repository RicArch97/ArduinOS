/*
    Common types and constants for ArduinOS.

    Created by Ricardo Steijn on 18-05-2021.
*/

#ifndef COMMON_H
#define COMMON_H

#define ARG_NAMESIZE        64
#define MAX_ARG_COUNT       3

typedef struct {
    char args[MAX_ARG_COUNT][ARG_NAMESIZE];
} CommandArgs;

#endif