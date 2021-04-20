#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <utime.h>
#include <time.h>
#include <syslog.h>
#include "linkedList.h"
#include "copy.h"
#include "dirChecker.h"
#include "utils.h"

// "source file" - file inside of source directory
// "target file" - file inside of target directory

int err = 0;

typedef struct directory
{
    char *path;
    DIR *pointer;
    node *file_list;
} directory;

void checkDirectories(char *source_path, char *target_path)
{
    sendLog(LOG_INFO, "Comparing directories...", 0);

    directory source_dir;
    directory target_dir;
    char sourcef_path[PATH_MAX + 1];
    char targetf_path[PATH_MAX + 1];
    struct dirent *entry;
    struct stat source_f, target_f;
    char *fileName;
    struct utimbuf modify_time;

    source_dir.path = source_path;
    target_dir.path = target_path;
    source_dir.pointer = opendir(source_dir.path);
    target_dir.pointer = opendir(target_dir.path);
    source_dir.file_list = NULL;
    target_dir.file_list = NULL;

    // kopiuj sciezke do sourcef_path
    strcpy(sourcef_path, source_dir.path);
    size_t sourcef_path_len = strlen(source_dir.path);
    strcpy(targetf_path, target_dir.path);
    size_t targetf_path_len = strlen(target_dir.path);

    fixDirectorySyntax(sourcef_path, &sourcef_path_len);
    fixDirectorySyntax(targetf_path, &targetf_path_len);

    // jak wskaznik nie jest nullem to bierz wszystko do listy
    if(source_dir.pointer != NULL)
    {
        errno = 0;
        while (entry = readdir(source_dir.pointer))
        {
            if(errno != 0)
                checkErrorsFile(-1, "Can't open source directory.", source_dir.path);

            source_dir.file_list = add(source_dir.file_list, entry->d_name);
        }
    }
    else
    {
        sendLog(LOG_ERR, "Can't open source directory.", 0);
        exit(EXIT_FAILURE);
    }

    // jak wskaznik nie jest nullem to bierz wszystko do listy
    if(target_dir.pointer != NULL)
    {
        errno = 0;
        while (entry = readdir(target_dir.pointer))
        {
            if(errno != 0)
                checkErrorsFile(-1, "Can't open target directory.", target_dir.path);

            target_dir.file_list = add(target_dir.file_list, entry->d_name);
        }
    }
    else
    {
        sendLog(LOG_ERR, "Can't open target directory.", 0);
        exit(EXIT_FAILURE);
    }
    
    // dla kazdego pliku w liscie
    while (source_dir.file_list)
    {
        fileName = source_dir.file_list->fileName;
        // do nazwy sciezki doklej nazwe pliku
        strncpy(sourcef_path + sourcef_path_len, fileName, sizeof(sourcef_path) - sourcef_path_len);
        // dane pliku source
        err = lstat(sourcef_path, &source_f);
        checkErrorsFile(err, "Couldn't read source file stats.", sourcef_path);
        
        // czy zwykly plik
        if (S_ISREG(source_f.st_mode))
        {
            strncpy(targetf_path + targetf_path_len, fileName, sizeof(targetf_path) - targetf_path_len);
            // czy plik jest w targetdir
            if(search(target_dir.file_list, fileName) == 0)
            {
                // dane pliku target
                err = lstat(targetf_path, &target_f);
                checkErrorsFile(err, "Couldn't read target file stats.", targetf_path);

                // jesli sa takie same permissions
                if(source_f.st_mode == target_f.st_mode)
                {
                    // rozne czasy modyfikacji
                    if(source_f.st_mtime != target_f.st_mtime)
                    {
                        copy(sourcef_path, targetf_path, source_f.st_mode, source_f.st_size);
                    }
                }
                else
                {
                    // usun target plik
                    err = unlink(targetf_path);
                    checkErrorsFile(err, "Couldn't delete target file.", targetf_path);
                    
                    // kopiuj
                    copy(sourcef_path, targetf_path, source_f.st_mode, source_f.st_size);
                }
                // usuwanie pliku z listy target_dir
                target_dir.file_list = removeNode(target_dir.file_list, fileName);
            }
            else
            {
                // kopiuj plik i wyslij permissions
                copy(sourcef_path, targetf_path, source_f.st_mode, source_f.st_size);
            }
            // access time i modify time
            modify_time.actime = source_f.st_atime;
            modify_time.modtime = source_f.st_mtime;
            // zaktualizuj czas w target pliku
            err = utime(targetf_path, &modify_time);
            checkErrorsFile(err, "Couldn't change last access/modification time.", targetf_path);
        }
        // usuniecie pliku z listy source dir
        source_dir.file_list = pop(source_dir.file_list);
    }

    while (target_dir.file_list)
    {
        fileName = target_dir.file_list->fileName;
        if(strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0)
        {
            strncpy(targetf_path + targetf_path_len, fileName, sizeof(targetf_path) - targetf_path_len);
            // tu trzeba sprawdzic czy plik w target dir to katalog czy plik
            err = unlink(targetf_path);
            checkErrorsFile(err, "Can't delete target file.", targetf_path);

            sendLog(LOG_INFO, "Successfully deleted file.", 0);
        }

        target_dir.file_list = pop(target_dir.file_list);
    }
}
