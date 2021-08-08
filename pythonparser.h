#ifndef PYTHONPARSER_H
#define PYTHONPARSER_H

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

class PythonParser
{
public:
    PythonParser();
    static int parse_python_dictionary(std::vector<std::string> &all, std::string str);
    static std::string write_python_dictionary(std::vector<std::string> &all, int size);
    static void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

public:
    enum {
         hugepp, // don't touch it
         coursename,
         coursename_data,
         credit,
         credit_data,
         letter,
         letter_data,
         grade,
         grade_data,
         weight,
         weight_data
    };
};

#endif // PYTHONPARSER_H
