#include "utility.h"

const std::vector<std::string> Utility::VALID_LETTERS = {"AA", "BA", "BB", "CB", "CC", "DC", "DD", "FD", "FF", "NA", "U", "P", "W", "I", "EX", "S"};
const QString Utility::ALLOWED_SYMBOLS = QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");


Utility::Utility()
{
}

bool Utility::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void Utility::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


QString Utility::random_string(int length)
{

    QString result;
    qsrand(QTime::currentTime().msec());
    for (int i = 0; i < length; ++i) {
        result.append(ALLOWED_SYMBOLS.at(qrand() % (ALLOWED_SYMBOLS.length())));
    }
    return result;
}

double Utility::calculate_grade(std::string letter_add)
{
    unsigned int index;
    double grade_add;

    index = std::distance(VALID_LETTERS.begin(), std::find(VALID_LETTERS.begin(), VALID_LETTERS.end(), letter_add));

    if (index>=8 and index<=13)
        grade_add = 0;
    else if (index>= 14 and index<=15)
        grade_add = 4;
    else
        grade_add =  (4 - 0.5*index);
    return grade_add;
}

bool Utility::fileExists(QString m_path)
{
    QFileInfo check_file(m_path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}
