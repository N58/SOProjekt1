#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

// Global variables
unsigned long optionalSize = 31457280; // 30MB
unsigned int optionalTime = 300; // 5 min
unsigned char recursive = 0;

void fixDirectorySyntax(char* source, size_t* length)
{
    if (source[*length - 1] != '/') {
        source[*length] = '/';
        source[*length + 1] = '\0';
        ++*length;
    }
}

void sendLog(int facility_priority, char* message, int sendErrno)
{
    if(sendErrno)
        syslog(facility_priority, "%s Error: %s.", message, strerror(errno));
    else
        syslog(facility_priority, "%s", message);

    closelog();
}

void sendLogFile(int facility_priority, char* message, char* source, char* target, int sendErrno)
{
    char msg[255];
    strcpy(msg, message);
    if(source != NULL)
    {
        strcat(msg, " Source: ");
        strcat(msg, source);
        strcat(msg, ".");
    }

    if(target != NULL)
    {
        strcat(msg, " Target: ");
        strcat(msg, target);
        strcat(msg, ".");
    } 

    sendLog(facility_priority, msg, sendErrno);
}

void checkErrors(int err, char* message)
{
    if(err == -1)
    {
        sendLog(LOG_ERR, message, 1);
        exit(EXIT_FAILURE);
    }
}

void checkErrorsNoErrno(int err, char* message)
{
    if(err == -1)
    {
        sendLog(LOG_ERR, message, 0);
        exit(EXIT_FAILURE);
    }
}

void checkErrorsFile(int err, char* message, char* filename)
{
    if(err == -1)
    {
        char msg[255];
        strcpy(msg, message);
        strcat(msg, " File name: `");
        strcat(msg, filename);
        strcat(msg, "`.");
        sendLog(LOG_ERR, msg, 1);
        exit(EXIT_FAILURE);
    }
}

/*char* getDirectoryInfo(char* source, char* target)
{
    char *result = (char*)malloc(sizeof(char) * 255);
    strcpy(result, " Source directory: ");
    strcat(result, source);
    strcat(result, ". Target directory: ");
    strcat(result, target);
    strcat(result, ".");
    return result;
}*/
