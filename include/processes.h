/*
    Header for the processes of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#ifndef PROCESSES_H
#define PROCESSES_H

void suspend(CommandArgs args);
void resume(CommandArgs args);
void kill(CommandArgs args);

void id();

#endif