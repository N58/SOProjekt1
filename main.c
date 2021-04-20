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

void Refresh(int signum);

int main(int argc, char* argv[]) {
    
    // Checking if there are at least 2 arguments
    if(argc < 3) {
        printf("This program requires 2 arguments. '%s [sourceDirectory] [targetDirectory] [time:optional]'", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* sourceDir = argv[1];
    char* targetDir = argv[2];
    checkDirectories(sourceDir, targetDir);

    
    /*// Setting and validating source directory
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
    
    // Setting and validating optional time in seconds
    if((int)atoi(argv[3]) < 1 && argv[3] != NULL) {
        printf("Incorrect optional time!\n");
        exit(EXIT_FAILURE);
    }
    else
        optionalTime = (int)atoi(argv[3]);

    // Setting up handler
    if(signal(SIGUSR1, Refresh) == SIG_ERR) {
        printf("Error while setting handler: %s\n", strerror(errno));
    }

    // Our process ID and Session ID 
    pid_t pid, sid;
    
    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        sendLog(LOG_ERR, "Forking error.");
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
            sendLog(LOG_ERR, "Error while setting up new SID.");
            exit(EXIT_FAILURE);
    }
    
    // Change the current working directory
    if ((chdir("/")) < 0) {
            sendLog(LOG_ERR, "Error while changing current directory.");
            exit(EXIT_FAILURE);
    }
    
    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Daemon-specific initialization goes here
    sendLog(LOG_INFO, "Daemon started succesfully.");

    while (1) {
        Refresh(SIGUSR1);
        sleep(optionalTime);
    }
   exit(EXIT_SUCCESS);
}

// Refresh handler (SIGUSR1 runs on "kill -10 <pid>")
void Refresh(int signum){
    if(signum == SIGUSR1)
    {
        sendLog(LOG_INFO, "Directory refreshed.");
        // TODO
    }*/
}