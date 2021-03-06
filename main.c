#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include "dirChecker.h"
#include "utils.h"

void Refresh();
char* sourceDir;
char* targetDir;

int main(int argc, char* argv[]) {
    
    // Checking if there are at least 2 arguments
    if(argc < 3) {
        printf("This program requires 2 arguments. '%s [sourceDirectory] [targetDirectory] [recursive:optional, time:optional, size:optional]'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sourceDir = argv[1];
    targetDir = argv[2];
    
    int c;
    while ((c = getopt(argc, argv, "RT:S:")) != -1)
    {
        switch (c)
        {
            case 'R': 
                recursive = 1;
                break;
            case 'T':
                if((int)atoi(optarg) < 1 || optarg == NULL)
                {
                    printf("Optional time cannot be lower than 1.\n");
                    exit(EXIT_FAILURE);
                }
                optionalTime = (int)atoi(optarg);
                break;
            case 'S':
                if((int)atoi(optarg) < 0 || optarg == NULL)
                {
                    printf("Optional size cannot be lower than 1.\n");
                    exit(EXIT_FAILURE);
                }
                optionalSize = (int)atoi(optarg);
                break;
            case '?': 
                if(optopt != 'S' && optopt != 'T')
                {
                    printf("Unknown option: %c\n", optopt);
                    exit(EXIT_FAILURE);
                }
                break; 
            default:
                continue;
        }
    }
    
    // Setting and validating source directory
    struct stat sb1;
    if (stat(sourceDir, &sb1) != 0 || !S_ISDIR(sb1.st_mode)) {
        printf("No such source directory.\n");
        exit(EXIT_FAILURE);
    }

    // Setting and validating target directory
    struct stat sb2;
    if (stat(targetDir, &sb2) != 0 || !S_ISDIR(sb2.st_mode)) {
        printf("No such target directory.\n");
        exit(EXIT_FAILURE);
    }

    // Setting up handler
    if(signal(SIGUSR1, Refresh) == SIG_ERR) {
        printf("Error while setting handler: %s\n", strerror(errno));
    }

    // Our process ID and Session ID 
    pid_t pid, sid;
    
    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        sendLog(LOG_ERR, "Forking error." , 1);
        exit(EXIT_FAILURE);
    }
    // If we got a good PID, then we can exit the parent process.
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    // Change the file mode mask 
    umask(0);
            
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
            sendLog(LOG_ERR, "Error while setting up new SID.", 1);
            exit(EXIT_FAILURE);
    }
    
    // Change the current working directory
    if ((chdir("/")) < 0) {
            sendLog(LOG_ERR, "Error while changing current directory.", 1);
            exit(EXIT_FAILURE);
    }
    
    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Daemon-specific initialization goes here
    sendLogFile(LOG_INFO, "Daemon started succesfully.", sourceDir, targetDir, 0);

    while (1) {
        Refresh();
        sleep(optionalTime);
    }
   exit(EXIT_SUCCESS);
}

// Refresh handler (SIGUSR1 runs on "kill -10 <pid>")
void Refresh(){
    sendLog(LOG_INFO, "DAEMON WOKE UP.", 0);
    checkDirectories(sourceDir, targetDir);
    sendLog(LOG_INFO, "DAEMON FELL ASLEEP.", 0);
}