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

#define DEFAULT_TIME 300

void Refresh(int signum);

int main(int argc, char* argv[]) {

    char* sourceDir = argv[1];
    char* targetDir = argv[2];
    checkDirectories(sourceDir, targetDir);
    unsigned int optionalTime;

    /*// Setting and validating source directory
    struct stat sb1;
    if (stat(sourceDir, &sb1) != 0 || !S_ISDIR(sb1.st_mode)) {
        printf("Katalog źródłowy nie istnieje!\n");
        exit(EXIT_FAILURE);
    }

    // Setting and validating target directory
    struct stat sb2;
    if (stat(targetDir, &sb2) != 0 || !S_ISDIR(sb2.st_mode)) {
        printf("Katalog docelowy nie istnieje!\n");
        exit(EXIT_FAILURE);
    }
    
    // Setting and validating optional time in seconds
    if(argv[3] == NULL)
        optionalTime = DEFAULT_TIME;
    else if( (int)atoi(argv[3]) < 1) {
        printf("Podano nieprawidlowy opcjonalny czas!\n");
        exit(EXIT_FAILURE);
    }
    else
        optionalTime = (int)atoi(argv[3]);

    // Setting up handler
    if(signal(SIGUSR1, Refresh) == SIG_ERR) {
        printf("Blad ustawiania handlera: %s\n", strerror(errno));
    }

    // Our process ID and Session ID 
    pid_t pid, sid;
    
    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Forking error.");
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
            syslog(LOG_ERR, "Error while setting up new SID.");
            exit(EXIT_FAILURE);
    }
    
    // Change the current working directory
    if ((chdir("/")) < 0) {
            syslog(LOG_ERR, "Error while changing current directory.");
            exit(EXIT_FAILURE);
    }
    
    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Daemon-specific initialization goes here
    syslog(LOG_INFO, "Daemon started succesfully.");

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
        syslog(LOG_INFO, "Directory refreshed.");
        // TODO
    }*/
}