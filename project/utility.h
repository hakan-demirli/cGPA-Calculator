#ifndef UTILITY_H
#define UTILITY_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QStandardPaths>
#include <QtGlobal>
#include <QTime>

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <sstream>

class Utility
{
public:
    Utility();
    static bool replace(std::string& str, const std::string& from, const std::string& to);
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    static QString random_string(int length = 13);
    static double calculate_grade(std::string letter_add);
    static bool fileExists(QString m_path);

public:
    static const std::vector<std::string> VALID_LETTERS;
    static const QString ALLOWED_SYMBOLS;
};

#endif // UTILITY_H
