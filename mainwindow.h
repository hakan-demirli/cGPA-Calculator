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
#include <QtAndroid>
#include <QScroller>

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
#include "utility.h"


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

    void on_deleteButton_clicked();

    void on_folderBrowser_cursorPositionChanged();

    void on_addFile_clicked();

    void on_deleteFile_clicked();

private:
    void file_open_and_read();

    void update_textBrowser();

    void update_file_list();

    void update_file_browser();

    void html_add_row(std::string& str);

    void update_textBrowser_block_number();

    void initialize_file_structure();

    void update_cgpa();

public:
    static const QString APPDATA_PATH;
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
