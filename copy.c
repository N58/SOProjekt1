#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "copy.h"

void copy(char* sourceName, char* targetName)
{
	int i, sourceFile, targetFile;
	size_t bytes_read, bytes_write;
	unsigned char buffer[16];
	
	sourceFile = open(sourceName, O_RDONLY);
	if(sourceFile == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	
	targetFile = open(targetName, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if(targetFile == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	do
	{
		bytes_read = read(sourceFile, buffer, sizeof(buffer));
		if(bytes_read == -1)
		{
			perror("Error reading file");
			exit(EXIT_FAILURE);
		}
		bytes_write = write(targetFile, buffer, bytes_read); 
		if(bytes_read == -1)
		{
			perror("Error writing file");
			exit(EXIT_FAILURE);
		}
		if(bytes_read != bytes_write)
		{
			printf("Error coping file");
			exit(EXIT_FAILURE);
		}
	}
	while(bytes_read != 0);
	
	if(close(sourceFile) == -1 || close(targetFile) == -1)
	{
		perror("Error closing file");
		exit(EXIT_FAILURE);
	}	
}
