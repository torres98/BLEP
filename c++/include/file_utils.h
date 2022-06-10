#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <iostream>

std::string read(const char *file_path);

unsigned int get_file_size(FILE* f);

#endif
