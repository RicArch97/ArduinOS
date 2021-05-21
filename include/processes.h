/*
    Header for the processes of ArduinOS.

    Created by Ricardo Steijn on 17-05-2021.
    Last update on 17-05-2021.
*/

#ifndef PROCESSES_H
#define PROCESSES_H

void run(CommandArgs *argv);
void list(CommandArgs *argv);
void suspend(CommandArgs *argv);
void resume(CommandArgs *argv);
void kill(CommandArgs *argv);

#endif