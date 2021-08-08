#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QStandardPaths>
#include <QtGlobal>
#include <QTime>
#include <QStyle>
#include <QFontDatabase>

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "pythonparser.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_textBrowser_cursorPositionChanged();

    void on_addButton_clicked();

    void on_fileLoadButton_clicked();

    void on_deleteButton_clicked();

    void on_folderBrowser_cursorPositionChanged();

    void on_addFile_clicked();

    void on_deleteFile_clicked();

private:
    void file_open_and_read(QString &);
    static bool fileExists(QString);
    void update_textBrowser();
    double calculate_grade(std::string);
    void file_Load();
    void update_file_list();
    static QString random_string(int);
    void update_file_browser();

private:
    const QString m_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation); // + ("/gpa");
    const std::vector<std::string> m_validLetters = {"AA", "BA", "BB", "CB", "CC", "DC", "DD", "FD", "FF", "NA", "U", "P", "W", "I", "EX", "S"};
    const std::string default_py_dic = R"({"data": [{"courseName": "CENG", "credit": 0.0, "letter": "FF", "grade": 0.0, "weight": 0.0}], "date": "2021-08-07_13.59.30"})";
    //const std::vector<std::string> m_creditZeroLetters = {"U", "P", "W", "I", "EX", "S"};

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
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
