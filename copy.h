#ifndef COPY_H
#define COPY_H

void copy(char* sourceName, char* targetName, mode_t mode, off_t size);
void copyLarge(char* sourceName, char* targetName, mode_t mode, off_t size);

#endif
