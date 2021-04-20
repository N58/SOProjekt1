#ifndef UTILS_H
#define UTILS_H

// Global variables
unsigned long optionalSize;
unsigned int optionalTime;

void fixDirectorySyntax(char* source, size_t* length);
void sendLog(int facility_priority, char* message, int sendErrno);
void checkErrors(int err, char* message);
void checkErrorsNoErrno(int err, char* message);
void checkErrorsFile(int err, char* message, char* filename);
char* getDirectoryInfo(char* source, char* target);

#endif