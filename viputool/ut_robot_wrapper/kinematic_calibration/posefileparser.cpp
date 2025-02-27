#include "ut_robot_wrapper/kinematic_calibration/posefileparser.hpp"
#include <regex>
#include <fstream>
#include <codecvt>
#include <locale>
#include <filesystem>

// #define LOG_ACTIVE_POSE_FILE_PARSE

PoseFileParser::PoseFileParser()
{
    Contruct_Regular_Expression();
}

PoseFileParser &PoseFileParser::set_File_Encoder_Type(EncoderType encoderType)
{
    this->encoder_type_ = encoderType;
    return *this;
}

PoseFileParser &PoseFileParser::set_double_splitter(std::string &splitter)
{
    this->splitter_ = splitter;
    Contruct_Regular_Expression();
    return *this;
}

PoseFileParser &PoseFileParser::set_double_number_per_line(int float_number_per_line)
{
    this->double_number_per_line_ = float_number_per_line;
    return *this;
}

int PoseFileParser::parse_double_vector(const std::string &file_path, std::vector<std::vector<double> > &data_vector_2d)
{
    data_vector_2d.clear();
    if(!std::filesystem::exists(file_path))
    {
        printf("%s - file '%s' not exist!!!", __FUNCTION__, file_path.c_str());
        return 0;
    }
    std::string str_line;
    if(EncoderType::UTF_8 == encoder_type_)
    {
        std::ifstream fin(file_path);
        int line_index{0};
        while(getline(fin, str_line))
        {
            line_index++;

            std::sregex_iterator next(str_line.begin(), str_line.end(), regular_expression_);
            std::sregex_iterator end;

            long float_number_dectected = std::distance(next, end);
            if(float_number_dectected != this->double_number_per_line_)
            {
#ifdef LOG_ACTIVE_POSE_FILE_PARSE
                printf("Line {%d} expect [%d] float number, actual [%ld] are decteted!\r\n",
                       line_index, this->double_number_per_line_, float_number_dectected);
                fflush(nullptr);
#endif
                continue;
            }
            std::vector<double> lineVector;
            while (next != end) {
                std::smatch match = *next;
                //float number is the 1st catpure group of regexp;
                double dreal = std::stof(match.str(1));
                lineVector.push_back(dreal);
                next++;
            }
            data_vector_2d.push_back(lineVector);
        }
    }
    else if(EncoderType::UTF_16LE == encoder_type_)
    {
        std::wifstream fin(file_path);
        fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
        int line_index{0};
        std::wstring wstr;
        while(getline(fin, wstr))
        {
            line_index++;
            str_line = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(wstr);

            std::sregex_iterator next(str_line.begin(), str_line.end(), regular_expression_);
            std::sregex_iterator end;

            long float_number_dectected = std::distance(next, end);
            if(float_number_dectected != this->double_number_per_line_)
            {
                printf("Line {%d} expect [%d] float number, actual [%ld] are decteted!\r\n",
                       line_index, this->double_number_per_line_, float_number_dectected);
                continue;
            }
            std::vector<double> lineVector;
            while (next != end) {
                std::smatch match = *next;
                //float number is the 1st catpure group of regexp;
                lineVector.push_back(std::stof(match.str(1)));
                next++;
            }
            data_vector_2d.push_back(lineVector);
        }
    }
    else
    {
        printf("%s - encoder type {%d} not support right now!!!", __FUNCTION__, (int)encoder_type_);
    }
#ifdef LOG_ACTIVE_POSE_FILE_PARSE
    printf("%s - extract 2D-vector[%d][%ld]\r\n", __FUNCTION__, double_number_per_line_, data_vector_2d.size());
#endif
    return data_vector_2d.size();
}

void PoseFileParser::Contruct_Regular_Expression()
{
    std::string str_regex = "(?:^|" + splitter_ + ")" + double_number_matcher;
    regular_expression_ = str_regex;
}
