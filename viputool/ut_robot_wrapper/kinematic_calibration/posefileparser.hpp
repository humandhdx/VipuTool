#ifndef POSEFILEPARSER_HPP
#define POSEFILEPARSER_HPP

#include <string>
#include <vector>
#include <regex>

#define POS_SPLITTER_DEFAULT "[,\t ]+"

class PoseFileParser
{
public:
    PoseFileParser();

    enum class EncoderType{
        UTF_8,
        UTF_16LE,
        // UTF_16BE
    };

    PoseFileParser &set_File_Encoder_Type(EncoderType encoderType);
    PoseFileParser &set_double_splitter(std::string& splitter);
    PoseFileParser &set_double_number_per_line(int double_number_per_line);

    /**
     * @brief parse_double_vector
     * parse the given file to get the 2D 'double' array
     * the column number is fixed by 'set_float_number_per_line'
     * @param file_path file path
     * @param data_vector_2d the parsed 2D 'double' array stored
     * @return the row size of 2D double vector
     */
    int parse_double_vector(const std::string &file_path, std::vector<std::vector<double>> &data_vector_2d);

private:
    //capture group for float/double number
    const std::string double_number_matcher = "([+-]?[0-9]*[.]?[0-9]+)";

    std::string splitter_               = POS_SPLITTER_DEFAULT;
    int         double_number_per_line_ = 6;
    EncoderType encoder_type_           = EncoderType::UTF_8;

    std::regex regular_expression_;
    void Contruct_Regular_Expression();
};

#endif // POSEFILEPARSER_HPP
