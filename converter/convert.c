/* convert
 * April 2021
 * Wouter Bergmann Tiest
 * 
 * Converts a text file in bytecode-language into a binary file and uploads
 * this to an Arduino running ArduinOS using the "erase" and "store" commands.
 * 
 * Usage: convert <file> <serial port>
 * 
 * Compilation with gcc or clang on Windows, Linux or MacOS:
 * gcc -o convert convert.c
 */
#define BUFSIZE 128
#define PROGSIZE 255
#define C_CHAR 1
#define C_INT 2
#define C_STRING 3
#define C_FLOAT 4
#define C_IF 128
#define C_ELSE 129
#define C_WHILE 131

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "instruction_array.h"

#ifdef _WIN32
#include <windows.h>
#define BPS 9600

// Read characters from serial stream pointed to by h until timeout
// Copy characters into buffer
// Append a terminating zero
// Return number of characters read
int readLine(HANDLE h, char *buffer) {
    DWORD bytesRead = 0;
    do {
        ReadFile(h, buffer, BUFSIZE, &bytesRead, NULL);
    } while (!bytesRead);
    buffer[bytesRead] = '\0';
    return (int)bytesRead;
}

// Write a buffer to serial stream pointed to by h
// Return number of characters written
int writeBuffer(HANDLE h, char *buffer, int noOfBytes) {
    DWORD bytesWritten;
    WriteFile(h, buffer, noOfBytes, &bytesWritten, NULL);
    return (int)bytesWritten;
}

// Append a newline character to buffer
// Write to serial stream pointed to by h
// Return number of characters written
int writeLine(HANDLE h, char *buffer) {
    strcat(buffer, "\n");
    return writeBuffer(h, buffer, strlen(buffer));
}
#else // Linux and MacOS
#include <fcntl.h>
#include <termios.h>
#define BPS B9600

// Read characters from serial stream pointed to by h until a newline character is read
// Copy characters into buf
// Append a terminating zero
// Return number of characters read
ssize_t readLine(int h, char *buf) {
    ssize_t bytesRead, n = 0;
    while (1) {
        bytesRead = read(h, buf, 1);
        if (bytesRead > 0) {
            if (*buf == '\n') {
                *(buf + 1) = '\0';
                return n;
            }
            buf += bytesRead;
            n += bytesRead;
        }
    }
}

// Write buffer to serial stream pointed to by h
// Return number of characters written
ssize_t writeBuffer(int h, unsigned char *buffer, int noOfBytes) {
    return write(h, buffer, noOfBytes);
}

// Append a newline character to buffer
// Write to serial stream pointed to by h
// Return number of characters written
ssize_t writeLine(int h, char *buffer) {
    strcat(buffer, "\n");
    return write(h, buffer, strlen(buffer));
}
#endif

// Return true if character is space, tab, carriage return or newline
int isWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Read a single word or multiple words within quotes from file into buf
// Return EOF if file has ended; otherwise 0
// Note: cannot deal with escaped or unbalanced quote marks
int readToken(FILE *file, char *buf) {
    // skip leading whitespace
    do {
        *buf = fgetc(file);
        if (*buf == EOF) return EOF;
    } while (isWhiteSpace(*buf));
    // start reading token
    char inQuote = 0;
    if (*buf == '\"') inQuote = 1;
    do {
        buf++;
        *buf = fgetc(file);
        if (*buf == '\"') inQuote = !inQuote;
    } while (*buf != EOF && (inQuote || !isWhiteSpace(*buf)));
    *buf = '\0'; // add terminating zero
    return 0;
}

// Convert the character after the backslash of an escaped character to the character
char unescape(char c) {
    switch (c) {
        case 'n':
            return '\n';
            break;
        case 'r':
            return '\r';
            break;
        case 't':
            return '\t';
            break;
        default: // for \\, \' and \"
            return c;
            break;
    }
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        printf("Usage: %s <file> <serial port>\n", argv[0]);
        return -1;
    }
    // open input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Cannot open file \"%s\"\n", argv[1]);
        return -1;
    }

    // process instructions
    printf("Converting file \"%s\"\n", argv[1]);
    char buf[BUFSIZE];
    unsigned char prog[PROGSIZE];
    int pc = 0;
    while (readToken(file, buf) != EOF) {
        int command = 0;
        if (*buf =='\'') { // char
            prog[pc++] = C_CHAR;
            if (buf[1] == '\\') {
                prog[pc++] = unescape(buf[2]);
            } else {
                prog[pc++] = buf[1];
            }
        }
        else if ((*buf >= '0' && *buf <= '9') || *buf == '.' || *buf == '-') { // number
            if (strchr(buf, '.')) { // float
                prog[pc++] = C_FLOAT;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                float *f = (float *)(prog + pc);
                *f = strtof(buf, NULL);
                pc += 4;
#else
                float f = strtof(buf, NULL);
                unsigned char *c = (unsigned char *)&f;
                for (int i = 3; i >= 0; i--) {
                    prog[pc++] = *(c + i);
                }
#endif
            } else if (!strncmp(buf, "0x", 2)) { // byte as hex
                prog[pc++] = strtol(buf, NULL, 16);
            } else { // int
                prog[pc++] = C_INT;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                short *s = (short *)(prog + pc);
                *s = (short)atoi(buf);
                pc += 2;
#else
                short s = atoi(buf);
                unsigned char *c = (unsigned char *)&s;
                prog[pc++] = *(c + 1);
                prog[pc++] = *c;
#endif
            }
        }
        else if (*buf == '\"') { // string
            prog[pc++] = C_STRING;
            for (int i = 1; i < strlen(buf) - 1; i++) {
                if (buf[i] == '\\') {
                    i++;
                    prog[pc++] = unescape(buf[i]);
                } else {
                    prog[pc++] = buf[i];
                }
            }  
            prog[pc++] = '\0'; // terminating zero
        }
        else { // command
            for (int i = 0; i < noOfInstr; i++) {
                if (!strcasecmp(buf, instrSet[i].name)) {
                    prog[pc++] = instrSet[i].number;
                    command = 1;
                    break;
                }
            }
            if (command) {
                if (prog[pc - 1] == C_IF || prog[pc - 1] == C_ELSE || prog[pc - 1] == C_WHILE) {
                    readToken(file, buf); // extra argument
                    prog[pc++] = atoi(buf);
                    if (prog[pc - 2] == C_WHILE) {
                        readToken(file, buf); // another extra argument
                        prog[pc++] = atoi(buf);
                    }
                }
            } else { // variable name
                prog[pc++] = *buf;
            }
        }
    }
    fclose(file);
    printf("Converted size = %d bytes\n", pc);

    // check serial port
#ifdef _WIN32
    HANDLE h = CreateFile(argv[2], GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) {
#else // Linux and MacOS
    int h = open(argv[2], O_RDONLY | O_NONBLOCK);
    if (h == -1) {
#endif
        printf("Cannot open port \"%s\".\n", argv[2]);
        return -1;
    }
#ifdef _WIN32
    CloseHandle(h);
#else // Linux and MacOS
    close(h);
#endif
    printf("Opening %s\n", argv[2]);

    // connect to Arduino
#ifdef _WIN32
    h = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    dcbSerialParams.BaudRate = BPS;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(h, &dcbSerialParams);
    COMMTIMEOUTS timeoutParams;
    timeoutParams.ReadIntervalTimeout = 2; // wait 2 ms for each character (9600 bps = 1.04 ms per character)
    SetCommTimeouts(h, &timeoutParams);
    EscapeCommFunction(h, SETDTR); // reset Arduino
    sleep(1);
    EscapeCommFunction(h, CLRDTR);
    sleep(1);
#else // Linux and MacOS
    h = open(argv[2], O_RDWR | O_NONBLOCK);
    struct termios settings;
    tcgetattr(h, &settings);
    cfsetispeed(&settings, BPS);
    cfsetospeed(&settings, BPS);
    settings.c_cflag |= CLOCAL; // ignore modem status lines
    tcsetattr(h, TCSANOW, &settings);
#endif
    readLine(h, buf); // wait for prompt
    printf("Sending file \"%s\"\n", argv[1]);
    snprintf(buf, BUFSIZE, "erase %s", argv[1]);
    writeLine(h, buf);
#ifdef _WIN32
    sleep(1);
#endif
    readLine(h, buf); // read answer and discard
    snprintf(buf, BUFSIZE, "store %s %d", argv[1], pc); 
    writeLine(h, buf);
    writeBuffer(h, prog, pc); // write data
#ifdef _WIN32
    sleep(1);
#endif
    readLine(h, buf); // read answer
    puts(buf);
#ifdef _WIN32
    CloseHandle(h);
#else // Linux and MacOS
    close(h);
#endif
}
