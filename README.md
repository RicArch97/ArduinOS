# ArduinOS

ArduinOS is a Unix like operating system for Arduino's. It has a CLI and has a filesystem with the use of EEPROM, memory and processing management.

This project is currently **a work in progress.**

## Deploying ArduinOS

You can open this project in VS Code using the PlatformIO extention. Simply click `upload & monitor` to run ArduinOS.

## Usage

```console
ArduinOS 0.1, A Unix like operating system for Arduino.
Usage: (command) (arg) ...

Commands:
store       <file> <size> <data>    Store a file in the filesystem.
retrieve    <file>                  Request a file from the filesystem.
erase       <file>                  Erase a file.
files                               List all files in the filesystem.
freespace                           Show the amount of free space in the filesystem.
run         <file>                  Run a program.
list                                Show a list with all processes.
suspend     <id>                    Suspend a process.
resume      <id>                    Resume a process.
kill        <id>                    Kill a process.
```

Simply execute a command by typing the command name, and arguments separated by spaces. The maximum amount of arguments that can be provided is 3.
For example, to store a file:

```console
$ store foo 9 foobar123
```

Note that the provided size is your own responsability. Providing a size that is too small, will only store characters up to the provided size.
A size that is too big, will fit the data just fine, but the space that isn't filled by data will be empty.

```console
$ store bar 20 barfoo456
```

Will fill 9 bytes, and the remaining 11 bytes will be empty. Keep in mind that the size is allocated and other file data cannot be written to the empty spaces.

In the current configuration, 10 files can be stored in the file system. This leaves 863 bytes available for data. You can use the `freespace` command
to see the maximum size of a file that can be stored. For example with 2 files of both 9 bytes:

```console
$ freespace
Free space available in filesystem: 845 bytes.
```
