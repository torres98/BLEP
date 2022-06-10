#include <fstream>
#include <sstream>

#include "../include/file_utils.h"

std::string read(const char *file_path) {
    
    std::ifstream file(file_path);
    std::stringstream file_content;

    if (file.fail()) {
        std::cout << "Error during read of " << file_path << std::endl;
        exit(-1);
    }

    file_content << file.rdbuf();

    file.close();
    
    return file_content.str();
}

unsigned int get_file_size(FILE* f) {
    fseek(f, 0, SEEK_END);
    unsigned int file_size = ftell(f);
    rewind(f);

    return file_size;
}
