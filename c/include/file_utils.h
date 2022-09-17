#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>


FILE* open_file(const char* filename, const char* modes);
size_t get_file_size(FILE* f);

#endif
