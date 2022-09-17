#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <iostream>

FILE* open_file(const char* filename, const char* mode);
size_t get_file_size(FILE* f);

#endif
