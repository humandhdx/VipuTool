#include "utils/ReadAndWriteFile.hpp"
#include <filesystem>
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>

void split(std::string &s, std::string &delim, std::vector<std::string>* ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::string::npos) {
        ret->push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
        ret->push_back(s.substr(last, index - last));
    }
}

void Utilities::read_file_data(std::vector<std::vector<double>> &res,
                               std::string path_and_file_name)
{
    std::ifstream readFile;
    std::string path_and_file = path_and_file_name;
    readFile.open(path_and_file, std::ios::in);
    std::filesystem::path filePath = path_and_file_name;
    std::string fileName = filePath.filename().string();
    if (readFile.is_open()) {
        //std::cout << "open " << fileName << " file successfully!" << std::endl;
        std::string str;
        std::vector<std::string> vec_s;
        std::vector<double> vec_d;
        std::string dot = ",";
        int i = 0;
        while (getline(readFile, str)) {
            vec_s.clear();
            vec_d.clear();
            //std::cout << str << std::endl;
            split(str, dot, &vec_s);
            for (auto elmt : vec_s) {
                try {
                    vec_d.push_back(stod(elmt));
                }
                catch (const std::exception &e) {
                    //std::cerr << e.what() << '\n';
                }
            }
            res.push_back(vec_d);
            i++;
        }
    }
    else {
        std::cout << "open file failed" << std::endl;
    }
    readFile.close();
}

void Utilities::write_file_data(std::vector<std::vector<double>> &res,
                                std::string path_and_file_name)
{
    std::ofstream outFile;
    time_t t = time(nullptr);
    //struct tm* now = localtime(&t);
    //std::stringstream timeStr;
    //timeStr << now->tm_year + 1900 << "_";
    //timeStr << now->tm_mon + 1 << "_";
    //timeStr << now->tm_mday << "_";
    //timeStr << now->tm_hour << "_";
    //timeStr << now->tm_min << "_";
    //timeStr << now->tm_sec;

    //std::string path = path_string + timeStr.str() + "data.txt";
    std::string path = path_and_file_name;
    outFile.open(path, std::ios::out | std::ios::trunc);  //write at the end of file
    if (!outFile.is_open()) {
        std::cout << "open file failed" << std::endl;
        return;
    }
    outFile << std::fixed << std::setprecision(10);
    for (auto elmt1 : res) {
        for (auto elmt2 : elmt1) {
            outFile << elmt2 << ",";
        }
        outFile << std::endl;
    }
    outFile.close();
}

extern const std::string Utilities::BIN_ABSOLUTE_PATH = Utilities::Get_BINARAY_ABS_PATH();

extern const std::string Utilities::BIN_ABSOLUTE_DIR_PATH = Utilities::Get_BINARAY_DIR_ABS_PATH();

std::string Utilities::Get_BINARAY_ABS_PATH()
{
    char path[PATH_MAX];
    std::string abs_path_bin;
    size_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count < 0) {
       std::cout << "Error reading symbolic link" << std::endl;
       return abs_path_bin;
    }
    path[count] = '\0';  //null terminate the string
    abs_path_bin = path;

    return abs_path_bin;
}

std::string Utilities::Get_BINARAY_DIR_ABS_PATH()
{
    char path[PATH_MAX];
    std::string abs_path_bin_dir;
    size_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count < 0) {
        std::cout << "Error reading symbolic link" << std::endl;
        return abs_path_bin_dir;
    }
    path[count] = '\0';  //null terminate the string
    char* dir = dirname(path);
    abs_path_bin_dir = dir;

    return abs_path_bin_dir;
}
