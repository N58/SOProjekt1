#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "linkedList.h"
#include "copy.h"
#include "dirChecker.h"

typedef struct directory
{
    char *path;
    DIR *pointer;
    node *file_list;
} directory;

void checkDirectories(char *source_patch, char *target_patch)
{
    directory source_dir;
    directory target_dir;
    char sourcef_path[PATH_MAX + 1];
    char targetf_path[PATH_MAX + 1];
    struct dirent *entry;
    struct stat st;
    char *fileName;

    source_dir.path = source_patch;
    target_dir.path = target_patch;
    source_dir.pointer = opendir(source_dir.path);
    target_dir.pointer = opendir(target_dir.path);
    source_dir.file_list = NULL;
    target_dir.file_list = NULL;

    if(source_dir.pointer != NULL)
    {
        while (entry = readdir(source_dir.pointer))
        {
            source_dir.file_list = add(source_dir.file_list, entry->d_name);
        }
    }
    else
    {
        perror("Can't open source directory");
        exit(EXIT_FAILURE);
    }

    if(target_dir.pointer != NULL)
    {
        while (entry = readdir(target_dir.pointer))
        {
            target_dir.file_list = add(target_dir.file_list, entry->d_name);
        }
    }
    else
    {
        perror("Can't open target directory");
        exit(EXIT_FAILURE);
    }

    strcpy(sourcef_path, source_dir.path);
    size_t sourcef_path_len = strlen(source_dir.path);
    strcpy(targetf_path, target_dir.path);
    size_t targetf_path_len = strlen(target_dir.path);
    
    while (source_dir.file_list)
    {
        fileName = source_dir.file_list->fileName;
        strncpy(sourcef_path + sourcef_path_len, fileName, sizeof(sourcef_path) - sourcef_path_len);
        lstat(sourcef_path, &st);
        if (S_ISREG(st.st_mode))
        {
            strncpy(targetf_path + targetf_path_len, fileName, sizeof(targetf_path) - targetf_path_len);
            if(search(target_dir.file_list, fileName) == 0)
            {
                copy(sourcef_path, targetf_path);
            }
            else
            {
                copy(sourcef_path, targetf_path);
            }
        }
        source_dir.file_list = pop(source_dir.file_list);
    }
    while (target_dir.file_list)
    {
        //usun co zostalo
    }
}
