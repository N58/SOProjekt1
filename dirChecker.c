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

void clearDirectory(char *path);
void removeDirOrFile(char *path, mode_t mode);
void checkDirectories(char *source_path, char *target_path);
void modifyTime(struct stat *source_f, char* targetf_path);

// "source file" - file inside of source directory
// "target file" - file inside of target directory

int err = 0;

//opis folderu
typedef struct directory
{
    char *path;
    DIR *pointer;
    node *file_list;
} directory;


//stwórz listę plików w folderze
void populateFileList(directory *dir)
{
    struct dirent *entry;
    dir->file_list = NULL;
    if (dir->pointer != NULL)
    {
        errno = 0;
        while (entry = readdir(dir->pointer))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            dir->file_list = add(dir->file_list, entry->d_name);
        }
        if (errno != 0)
            checkErrorsFile(-1, "Can't open directory.", dir->path);
    }
    else
        checkErrorsNoErrno(LOG_ERR, "Can't open directory.");
}

//wyczyść zawartość folderu
void clearDirectory(char *path)
{
    directory dir;
    struct stat stat;
    char dirf_path[PATH_MAX + 1];
    size_t dirf_len;

    dir.path = path;
    dir.pointer = opendir(dir.path);
    strcpy(dirf_path, dir.path);
    dirf_len = strlen(dir.path);
    fixDirectorySyntax(dirf_path, &dirf_len);

    populateFileList(&dir);

    while (dir.file_list)
    {
        strncpy(dirf_path + dirf_len, dir.file_list->fileName, sizeof(dirf_path) - dirf_len);
        err = lstat(dirf_path, &stat);
        checkErrorsFile(err, "Couldn't read target file stats.", dirf_path);
        removeDirOrFile(dirf_path, stat.st_mode);
        dir.file_list = pop(dir.file_list);
    }
}

void removeDirOrFile(char *path, mode_t mode)
{
    if (S_ISREG(mode))
    {
        err = unlink(path);
        checkErrorsFile(err, "Can't delete file.", path);
        sendLogFile(LOG_INFO, "Successfully deleted file.", NULL, path, 0);
    }
    else if (S_ISDIR(mode))
    {
        if(recursive)
        {
            clearDirectory(path);
            err = rmdir(path);
            checkErrorsFile(err, "Can't delete directory.", path);
            sendLogFile(LOG_INFO, "Successfully deleted directory.", NULL, path, 0);
        }
    }
    else
    {
        sendLogFile(LOG_INFO, "File is not standard file or directory. Cannot remove.", NULL, path, 0);
    }
}

void copyDirOrFile(char* source_path, char* target_path, struct stat *source, struct stat *target)
{
    if (S_ISREG(source->st_mode))
    {
        if(!target || source->st_mtime > target->st_mtime)
        {
            copy(source_path, target_path, source->st_mode, source->st_size);
            modifyTime(source, target_path);
        }
    }
    else if (S_ISDIR(source->st_mode))
    {
        if(recursive)
        {
            if(!target)
            {
                err = mkdir(target_path, source->st_mode);
                checkErrorsFile(err, "Couldn't make directory.", target_path);
                sendLogFile(LOG_INFO, "Successfully created directory.", NULL, target_path, 0);
            }
            checkDirectories(source_path, target_path);

            modifyTime(source, target_path);
        }
    }
    else
    {
        sendLogFile(LOG_INFO, "File is not standard file or directory. Cannot copy.", NULL, target_path, 0);
    }
}

void modifyTime(struct stat *source_f, char* targetf_path)
{
    struct utimbuf modify_time;
    modify_time.actime = source_f->st_atime;
    modify_time.modtime = source_f->st_mtime;
    // zaktualizuj czas w target pliku
    err = utime(targetf_path, &modify_time);
    checkErrorsFile(err, "Couldn't change last access/modification time.", targetf_path);
}

void checkDirectories(char *source_path, char *target_path)
{
    sendLogFile(LOG_INFO, "Comparing directories.", source_path, target_path, 0);

    directory source_dir;
    directory target_dir;
    char sourcef_path[PATH_MAX + 1];
    char targetf_path[PATH_MAX + 1];
    struct dirent *entry;
    struct stat source_f, target_f;
    char *fileName;

    source_dir.path = source_path;
    target_dir.path = target_path;
    source_dir.pointer = opendir(source_dir.path);
    target_dir.pointer = opendir(target_dir.path);

    // kopiuj sciezke do sourcef_path
    strcpy(sourcef_path, source_dir.path);
    size_t sourcef_path_len = strlen(source_dir.path);
    strcpy(targetf_path, target_dir.path);
    size_t targetf_path_len = strlen(target_dir.path);

    fixDirectorySyntax(sourcef_path, &sourcef_path_len);
    fixDirectorySyntax(targetf_path, &targetf_path_len);

    // jak wskaznik nie jest nullem to bierz wszystko do listy

    populateFileList(&source_dir);
    populateFileList(&target_dir);

    // dla kazdego pliku na liscie
    while (source_dir.file_list)
    {
        fileName = source_dir.file_list->fileName;
        // do nazwy sciezki doklej nazwe pliku
        strncpy(sourcef_path + sourcef_path_len, fileName, sizeof(sourcef_path) - sourcef_path_len);
        strncpy(targetf_path + targetf_path_len, fileName, sizeof(targetf_path) - targetf_path_len);
        // dane pliku source
        err = lstat(sourcef_path, &source_f);
        checkErrorsFile(err, "Couldn't read source file stats.", sourcef_path);

        if (search(target_dir.file_list, fileName) == 1)
        {
            err = lstat(targetf_path, &target_f);
            checkErrorsFile(err, "Couldn't read target file stats.", targetf_path);
            if (source_f.st_mode == target_f.st_mode)
            {
                copyDirOrFile(sourcef_path, targetf_path, &source_f, &target_f);
            }
            else
            {
                removeDirOrFile(targetf_path, target_f.st_mode);
                copyDirOrFile(sourcef_path, targetf_path, &source_f, NULL);
            }
            target_dir.file_list = removeNode(target_dir.file_list, fileName);
        }
        else
        {
            copyDirOrFile(sourcef_path, targetf_path, &source_f, NULL);
        }

        source_dir.file_list = pop(source_dir.file_list);
    }
    while (target_dir.file_list)
    {
        fileName = target_dir.file_list->fileName;
        {
            strncpy(targetf_path + targetf_path_len, fileName, sizeof(targetf_path) - targetf_path_len);
            err = lstat(targetf_path, &target_f);
            checkErrorsFile(err, "Couldn't read target file stats.", targetf_path);
            // tu trzeba sprawdzic czy plik w target dir to katalog czy plik
            removeDirOrFile(targetf_path, target_f.st_mode);
        }
        target_dir.file_list = pop(target_dir.file_list);
    }
    
    sendLogFile(LOG_INFO, "Finished comparing directories.", source_path, target_path, 0);
}
