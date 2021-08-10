#include "mainwindow.h"
#include "ui_mainwindow.h"

//const QString MainWindow::APPDATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
const QString MainWindow::APPDATA_PATH = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/cgpa";

const int MAX_COURSE_NAME_SIZE = 8;

const int MAX_CREDIT_SIZE = 4;

std::vector<std::string> all_data_list;
std::vector<std::string> fileList_vec;
QStringList fileList;
QString fileName_full;
QString line;

unsigned int all_data_size;
unsigned int block_number;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->credit_input->setInputMethodHints(Qt::ImhPreferNumbers);
    ui->letter_input->setInputMethodHints(Qt::ImhPreferUppercase);
    QFontDatabase::addApplicationFont(":/new/DroidSansMono.ttf");
    ui->textBrowser->setFont(QFont("DroidSansMono", 24, 1));
    ui->fileBrowser->setFont(QFont("DroidSansMono", 20, 1));

    ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);

    ui->fileBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->fileBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(ui->fileBrowser, QScroller::LeftMouseButtonGesture);

    initialize_file_structure();

    file_open_and_read();

    update_textBrowser();

    update_textBrowser_block_number();

    update_file_list();
    ui->fileBrowser->append(fileList.join("\n"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_textBrowser_cursorPositionChanged()
{
    update_textBrowser_block_number();
    QTextCursor cur = ui->textBrowser->textCursor();
    QTextBlockFormat f;
    //QString text;
    //f.setBackground(Qt::red);
    cur.select(QTextCursor::LineUnderCursor);
    cur.setBlockFormat(f);
    ui->textBrowser->setTextCursor(cur);
    //text = cur.selectedText();

    ui->course_input->setText(QString::fromStdString(all_data_list[coursename_data + (block_number*10)]));
    ui->letter_input->setText(QString::fromStdString(all_data_list[letter_data + (block_number*10)]));
    ui->credit_input->setText(QString::fromStdString(all_data_list[credit_data + (block_number*10)]));
    update_cgpa();
}

void MainWindow::on_addButton_clicked()
{
    std::vector <std::string> temp_valid_letters = Utility::VALID_LETTERS;
    unsigned int index_of_letter = std::distance(temp_valid_letters.begin(), std::find(temp_valid_letters.begin(), temp_valid_letters.end(), ui->course_input->text().toStdString()));

    if (index_of_letter >= temp_valid_letters.size())
        return;

    if(ui->course_input->text().length() > MAX_COURSE_NAME_SIZE ||
       ui->credit_input->text().length() > MAX_CREDIT_SIZE)
        return;

    if(std::stod(ui->credit_input->text().toStdString()) < 0)
        return;

    unsigned int index_of_course = std::distance(all_data_list.begin(), std::find(all_data_list.begin(), all_data_list.end(), ui->course_input->text().toStdString()));

    std::stringstream stream;
    double grade_add;
    std::string letter_add = ui->letter_input->text().toStdString();

    grade_add = Utility::calculate_grade(letter_add);
    stream << std::fixed << std::setprecision(1) << grade_add;
    std::string grade_add_str = stream.str();
    stream.str(std::string());

    std::string weight_str;
    stream << std::fixed << std::setprecision(1) << ((ui->credit_input->text().toDouble())*(grade_add));
    weight_str = stream.str();

    float temp = ((all_data_size + 0.5 )/10);
    all_data_size = 10 * floor(temp);

    if (index_of_course >= all_data_list.size()){
        if (all_data_list.size() <= all_data_size+10)
            all_data_list.resize(all_data_size+11);
        all_data_list[all_data_size+coursename] = "courseName";
        all_data_list[all_data_size+coursename_data] = ui->course_input->text().toStdString();
        all_data_list[all_data_size+credit] =  "credit";
        all_data_list[all_data_size+credit_data] = ui->credit_input->text().toStdString();
        all_data_list[all_data_size+letter] =  "letter";
        all_data_list[all_data_size+letter_data] = ui->letter_input->text().toStdString();
        all_data_list[all_data_size+grade] =  "grade";
        all_data_list[all_data_size+grade_data] = grade_add_str;
        all_data_list[all_data_size+weight] =  "weight";
        all_data_list[all_data_size+weight_data] = weight_str;
        all_data_size = all_data_size + 10;
    }
    else {
        all_data_list[coursename_data + (block_number*10)] =  ui->course_input->text().toStdString();
        all_data_list[letter_data + (block_number*10)] = ui->letter_input->text().toStdString();
        all_data_list[credit_data + (block_number*10)] = ui->credit_input->text().toStdString();
        all_data_list[grade_data + (block_number*10)] = grade_add_str;
        all_data_list[weight_data + (block_number*10)] = weight_str;
    }

    std::string python_dictionary;
    python_dictionary = PythonParser::write_python_dictionary(all_data_list, all_data_size);

    QFile file(fileName_full);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        exit(1);

    QTextStream outStream(&file);
    outStream << QString::fromStdString(python_dictionary);

    file.close();
    update_textBrowser();
}

void MainWindow::file_open_and_read()
{
    QFile file(fileName_full);
    try {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    } catch (std::exception &e) {
        std::cerr << "can't read file" << std::endl;
        std::cerr << e.what() << std::endl;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    file.close();

    try {
        all_data_size = PythonParser::parse_python_dictionary(all_data_list, line.toStdString());
    } catch (const std::bad_alloc &e) {
        std::cerr << "PythonParser error" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void MainWindow::update_textBrowser()
{
    std::string display_html =
                R"(
                    <table style="width: 100%">
                        <colgroup>
                           <col span="1" style="width: 25%;">
                           <col span="1" style="width: 25%;">
                           <col span="1" style="width: 25%;">
                           <col span="1" style="width: 25%;">
                        </colgroup>
                        <!-- (%r%r) these are row indicator comments. Don not delete -->
                        <tbody>
                            <!--%r%r-->
                        </tbody>
                    </table>
                )";
    try {
        std::stringstream cn_d;
        std::stringstream l_d;
        std::stringstream c_d;
        std::stringstream w_d;
        char fill_character = ' ';
        for(unsigned int i = 0; i < all_data_size - 9; i = i + 10){
            html_add_row(display_html);

            cn_d  << std::left  << std::setw(MAX_COURSE_NAME_SIZE) << std::setfill(fill_character) << all_data_list[coursename_data + i];
            l_d   << std::right << std::setw(7)  << std::setfill(fill_character) << all_data_list[letter_data + i];
            c_d   << std::right << std::setw(8)  << std::setfill(fill_character) << all_data_list[credit_data + i];
            w_d   << std::right << std::setw(8)  << std::setfill(fill_character) << all_data_list[weight_data + i] << std::right;
            Utility::replace(display_html, "%s", cn_d.str());
            Utility::replace(display_html, "%s", l_d.str());
            Utility::replace(display_html, "%s", c_d.str());
            Utility::replace(display_html, "%s", w_d.str());
            cn_d.str("");
            l_d.str("");
            c_d.str("");
            w_d.str("");
        }
        ui->textBrowser->setHtml(QString::fromStdString(display_html));

    } catch (std::exception &e) {
        std::cerr << "can't update textBrowser" << std::endl;
        std::cerr << e.what() << std::endl;
        return;
    }
}

void MainWindow::on_deleteButton_clicked()
{

    if (all_data_size - 10 < 5)
    {

    }else
    {
        all_data_size = all_data_size - 10;
        int index = block_number*10;
        int delete_size = 10;
        std::rotate(all_data_list.begin()+index, all_data_list.begin()+index+delete_size, all_data_list.end());
        all_data_list.resize(all_data_list.size()-10);

        std::string python_dictionary;

        python_dictionary = PythonParser::write_python_dictionary(all_data_list, all_data_size);

        QFile file(fileName_full);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            exit(1);

        QTextStream outStream(&file);
        outStream << QString::fromStdString(python_dictionary);

        file.close();

        update_textBrowser();
    }
}

void MainWindow::on_fileBrowser_cursorPositionChanged()
{
    update_file_list();
    unsigned int file_number = ui->fileBrowser->textCursor().blockNumber();
    QTextCursor cur = ui->fileBrowser->textCursor();
    QTextBlockFormat f;
    cur.select(QTextCursor::LineUnderCursor);
    cur.setBlockFormat(f);
    ui->fileBrowser->setTextCursor(cur);

    fileName_full = APPDATA_PATH + "/" +QString::fromStdString(fileList.at(file_number).toStdString());

    file_open_and_read();

    update_textBrowser();

}

void MainWindow::on_addFile_clicked()
{
    QDateTime currentDate = QDateTime::currentDateTime();
    fileName_full = APPDATA_PATH + "/" + currentDate.toString("yyyy-MM-dd_HH.mm.ss") + ".txt";

    if(!Utility::fileExists(fileName_full))
    {
        QFile file(fileName_full);
        try {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
        } catch (std::exception &e) {
            std::cerr << "can't create a default file" << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }
        QTextStream outStream(&file);
        outStream << QString::fromStdString(PythonParser::DEFAULT_PYTHON_DICTIONARY);
        file.close();
    }

    file_open_and_read();

    update_textBrowser_block_number();
    update_textBrowser();

    update_file_list();

    update_file_browser();
}

void MainWindow::on_deleteFile_clicked()
{
    if (fileList.size() <= 1 )     //|| 1 == ui->fileBrowser->textCursor().blockNumber())
    {
    }else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Ahy!", "Yer are deletin' dis file, ain't yer?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            std::cout << "0" <<std::endl;
            QFile file (fileName_full);
            try {
                file.remove();
            } catch (std::exception &e) {
                std::cerr << "file remove error" << std::endl;
                std::cerr << e.what() << std::endl;
            }
            std::cout << "1" <<std::endl;
            update_file_list();
            std::cout << "2" <<std::endl;
            update_file_browser();
            std::cout << "3" <<std::endl;
        }
        std::cout << "3" <<std::endl;
    }
}

void MainWindow::update_file_list(){
    QDir export_folder(APPDATA_PATH);
    export_folder.setNameFilters(QStringList()<<"*.txt"<<"*.json");
    fileList = export_folder.entryList();
}

void MainWindow::update_file_browser(){
    ui->fileBrowser->clear();
    ui->fileBrowser->append(fileList.join("\n"));
}

void MainWindow::html_add_row(std::string& str)
{
    std::string four_rows =
        R"(
                                <tr>
                                    <td style="text-align:  left;"><pre style="line-height:1.4">%s</pre></td>
                                    <td style="text-align: right;"><pre style="line-height:1.4">%s</pre></td>
                                    <td style="text-align: right;"><pre style="line-height:1.4">%s</pre></td>
                                    <td style="text-align: right;"><pre style="line-height:1.4">%s</pre></td>
                                </tr>
                                <!--%r%r-->
        )";
    Utility::replace(str,"<!--%r%r-->", four_rows);
}

void MainWindow::update_textBrowser_block_number()
{
    block_number = ui->textBrowser->textCursor().blockNumber();
    block_number = ((int)block_number-1)/4;
    if(block_number == (all_data_size/10))
    {
        block_number -= 1;
    }
}

void MainWindow::initialize_file_structure()
{
    QDir dir(APPDATA_PATH);
    if (!dir.exists())
        dir.mkdir(APPDATA_PATH);

    update_file_list();
    if (fileList.isEmpty()){
        QDateTime currentDate = QDateTime::currentDateTime();
        fileName_full = APPDATA_PATH + "/" + currentDate.toString("yyyy-MM-dd_HH.mm.ss") + ".txt";
    }
    else
        fileName_full = APPDATA_PATH + "/" + QString::fromStdString(fileList.at(0).toStdString());

    if(!Utility::fileExists(fileName_full))
    {


        QFile file(fileName_full);
        try {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
        } catch (std::exception &e) {
            std::cerr << "can't create a default file: " << e.what() <<std::endl;
            exit(1);
        }
        QTextStream outStream(&file);
        outStream << QString::fromStdString(PythonParser::DEFAULT_PYTHON_DICTIONARY);
        file.close();
    }
}

void MainWindow::update_cgpa()
{
    double sum_weight = 0;
    double sum_credit = 0;

    for(unsigned int i = 0; i < all_data_size - 9; i = i + 10){

        sum_weight += std::stod(all_data_list[weight_data + i]);
        sum_credit += std::stod(all_data_list[credit_data + i]);
    }
    std::stringstream cgpa;
    std::string cgpa_html =
        R"(
                <table style="width: 100%">
                  <colgroup>
                    <col span="1" style="width: 100%;">
                  </colgroup>
                  <!-- (%r%r) these are row indicator comments. Don not delete -->
                  <tbody>
                    <tr>
                      <td style="text-align:  center;"><pre style="line-height:1.0">%s</pre></td>
                    </tr>
                  </tbody>
                </table>
                )";

    if (1 > (sum_credit)){
        cgpa << "0";
    }else{
        cgpa << std::fixed << std::setprecision(3) << (sum_weight/sum_credit);
    }
    Utility::replace(cgpa_html, "%s", cgpa.str());
    ui->cgpaBrowser->setAlignment(Qt::AlignCenter);
    ui->cgpaBrowser->setHtml(QString::fromStdString(cgpa_html));
}
