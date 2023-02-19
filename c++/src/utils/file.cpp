#include <sstream>
#include <stdexcept>

#include "blep/utils/file.h"


FILE* open_file(const char* filename, const char* modes) {
    FILE* f = fopen(filename, modes);

    if (f == NULL) {
        std::ostringstream error_log;
        error_log << "Error during opening of file " << filename << " (modes " << modes << ")";   
        throw std::runtime_error(error_log.str());
    }
    
    return f;
}

size_t get_file_size(FILE* f) {
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    
    if (file_size == -1L)
        throw std::runtime_error("An error occurred while reading file size");

    rewind(f);

    return (size_t) file_size;
}
