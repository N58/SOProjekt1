#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <syslog.h>
#include "copy.h"
#include "utils.h"

void copy(char* sourceName, char* targetName, mode_t mode, off_t size)
{
	int i, sourceFile, targetFile;
	size_t bytes_read, bytes_write;
	unsigned char buffer[16];
	
	sourceFile = open(sourceName, O_RDONLY);
	checkErrorsFile(sourceFile, "Error opening source file.", sourceName);
	
	targetFile = open(targetName, O_WRONLY | O_CREAT | O_TRUNC, mode);
	checkErrorsFile(targetFile, "Error opening target file.", targetName);

	if(size >= optionalSize)
	{
		off_t offset = 0;
		size_t bytes_write = sendfile(targetFile, sourceFile, &offset, size);
		checkErrorsFile(bytes_write, "Error sending file.", targetName);
	}
	else
	{
		do
		{
			bytes_read = read(sourceFile, buffer, sizeof(buffer));
			checkErrorsFile(bytes_read, "Error reading source file.", sourceName);
			
			bytes_write = write(targetFile, buffer, bytes_read); 
			checkErrorsFile(bytes_write, "Error writing target file.", targetName);
			
			if(bytes_read != bytes_write)
				checkErrorsNoErrno(-1, "Error coping file. Reading and writing bytes are not equal"); // Forced uncommon error without errno
		}
		while(bytes_read != 0);
	}
	
	if(close(sourceFile) == -1)
		checkErrorsFile(-1, "Error closing source file.", sourceName); // Forced uncommon error 
	if(close(targetFile) == -1)
		checkErrorsFile(-1, "Error closing target file.", targetName); // Forced uncommon error 
	
	sendLogFile(LOG_INFO, "Successfully copied file.", sourceName, targetName, 0);
}