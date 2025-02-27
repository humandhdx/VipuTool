#ifndef READ_AND_WRITE_FILE_HPP_
#define READ_AND_WRITE_FILE_HPP_

#include <fstream>
#include <string>
#include <time.h>
#include <vector>

namespace Utilities {
void read_file_data(std::vector<std::vector<double>> &res, std::string path_and_file_name);
void write_file_data(std::vector<std::vector<double>> &res, std::string path_and_file_name);

std::string Get_BINARAY_ABS_PATH();

std::string Get_BINARAY_DIR_ABS_PATH();

extern const std::string BIN_ABSOLUTE_PATH;

extern const std::string BIN_ABSOLUTE_DIR_PATH;

};      //namespace Utilities
#endif  //READ_AND_WRITE_FILE_HPP_
