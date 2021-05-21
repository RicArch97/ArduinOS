/*
    Common types and constants for ArduinOS.

    Created by Ricardo Steijn on 18-05-2021.
*/

#ifndef COMMON_H
#define COMMON_H

#define ARG_NAMESIZE    64
#define MAX_ARG_AMOUNT  3

typedef struct {
    char arg[MAX_ARG_AMOUNT][ARG_NAMESIZE];
} CommandArgs;

#endif