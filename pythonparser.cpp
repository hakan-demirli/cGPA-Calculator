#include "pythonparser.h"

const std::string PythonParser::DEFAULT_PYTHON_DICTIONARY = R"({"data": [{"courseName": "CENG", "credit": 0.0, "letter": "FF", "grade": 0.0, "weight": 0.0}], "date": "1970-00-00_00.00.00"})";

PythonParser::PythonParser()
{

}

int PythonParser::parse_python_dictionary(std::vector<std::string> &all, std::string str)
{
    size_t pos = 0;
    std::string token;
    std::string debug;
    std::string delimiter = "\"";
    unsigned int i = 0;
    bool even = 1;
    while (pos != std::string::npos) {
        if (all.size() <= i)
              all.resize(i+1);
        if(even == 0) {
            pos = str.find(delimiter);
            str.erase(0, pos + delimiter.length());
            even = 1;
        }else {
            pos = str.find(delimiter);
            debug = str.substr(0, pos);
            all[i] = (str.substr(0, pos));
            all[i].erase(std::remove(all[i].begin(), all[i].end(), ':'), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), '}'), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), '{'), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), '['), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), ']'), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), '\"'), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), ','), all[i].end());
            all[i].erase(std::remove(all[i].begin(), all[i].end(), ' '), all[i].end());

            if (all[i].size() == 0)
                std::cout << "EMPTY" << std::endl;
            else     
                i += 1;

            even = 0;
        }
    }
    return i;
}

std::string PythonParser::write_python_dictionary(std::vector<std::string> &all, int size)
{
    std::string str;
    str = R"({"data": [)";
    for(int i = 0; i < size; i = i + 10) {
        str = str + R"({")";
        str = str + all[coursename + i] + R"(": ")" + all[coursename_data + i] +
                    R"(", ")" + all[credit + i] + R"(": )"  + all[credit_data + i] +
                    R"(, ")"  + all[letter + i] + R"(": ")" + all[letter_data + i] +
                    R"(", ")" + all[grade + i]  + R"(": )"  + all[grade_data + i]  +
                    R"(, ")"  + all[weight + i] + R"(": )"  + all[weight_data + i];
        str = str + "}, ";
    }
    str.pop_back();
    str.pop_back();
    str = str + R"(], "date": "2021-08-07_13.59.30"})";
    return str;
}

