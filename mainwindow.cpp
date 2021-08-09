#include "mainwindow.h"
#include "ui_mainwindow.h"

//const QString MainWindow::APPDATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
const QString MainWindow::APPDATA_PATH = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

const int MAX_COURSE_NAME_SIZE = 8;
const int MAX_LETTER_SIZE = 2;
const int MAX_CREDIT_SIZE = 4;

std::vector<std::string> all_data_list;
std::vector<std::string> fileList_vec;
QStringList fileList;
QString fileName_full;
QString line;

unsigned int all_list_size;
unsigned int block_number;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->credit_input->setInputMethodHints(Qt::ImhPreferNumbers);
    ui->letter_input->setInputMethodHints(Qt::ImhPreferUppercase);
    QFontDatabase::addApplicationFont(":/new/DroidSansMono.ttf");
    QFont font = QFont("DroidSansMono", 24, 1);
    ui->textBrowser->setFont(font);
    ui->folderBrowser->setFont(font);

    initialize_file_structure();

    file_open_and_read();

    update_textBrowser();

    update_textBrowser_block_number();

    update_file_list();
    ui->folderBrowser->append(fileList.join("\n"));
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
}

void MainWindow::on_addButton_clicked()
{
    if(ui->course_input->text().length() > MAX_COURSE_NAME_SIZE ||
       ui->letter_input->text().length() > MAX_LETTER_SIZE ||
       ui->credit_input->text().length() > MAX_CREDIT_SIZE)
    {
    }else
    {
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

        float temp = ((all_list_size + 0.5 )/10);
        all_list_size = 10 * floor(temp);

        if (index_of_course >= all_data_list.size()){
            if (all_data_list.size() <= all_list_size+10)
                all_data_list.resize(all_list_size+11);
            all_data_list[all_list_size+coursename] = "courseName";
            all_data_list[all_list_size+coursename_data] = ui->course_input->text().toStdString();
            all_data_list[all_list_size+credit] =  "credit";
            all_data_list[all_list_size+credit_data] = ui->credit_input->text().toStdString();
            all_data_list[all_list_size+letter] =  "letter";
            all_data_list[all_list_size+letter_data] = ui->letter_input->text().toStdString();
            all_data_list[all_list_size+grade] =  "grade";
            all_data_list[all_list_size+grade_data] = grade_add_str;
            all_data_list[all_list_size+weight] =  "weight";
            all_data_list[all_list_size+weight_data] = weight_str;
            all_list_size = all_list_size + 10;
        }
        else {
            all_data_list[coursename_data + (block_number*10)] =  ui->course_input->text().toStdString();
            all_data_list[letter_data + (block_number*10)] = ui->letter_input->text().toStdString();
            all_data_list[credit_data + (block_number*10)] = ui->credit_input->text().toStdString();
            all_data_list[grade_data + (block_number*10)] = grade_add_str;
            all_data_list[weight_data + (block_number*10)] = weight_str;
        }

        std::string python_dictionary;
        python_dictionary = PythonParser::write_python_dictionary(all_data_list, all_list_size);

        QFile file(fileName_full);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            exit(1);

        QTextStream outStream(&file);
        outStream << QString::fromStdString(python_dictionary);

        file.close();
        update_textBrowser();
    }
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
        all_list_size = PythonParser::parse_python_dictionary(all_data_list, line.toStdString());
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
        for(unsigned int i = 0; i < all_list_size - 9; i = i + 10){
            MainWindow::html_add_row(display_html);

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

    if (all_list_size - 10 < 5)
    {

    }else
    {
        all_list_size = all_list_size - 10;
        int index = block_number*10;
        int delete_size = 10;
        std::rotate(all_data_list.begin()+index, all_data_list.begin()+index+delete_size, all_data_list.end());
        all_data_list.resize(all_data_list.size()-10);

        std::string python_dictionary;

        python_dictionary = PythonParser::write_python_dictionary(all_data_list, all_list_size);

        QFile file(fileName_full);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            exit(1);

        QTextStream outStream(&file);
        outStream << QString::fromStdString(python_dictionary);

        file.close();

        update_textBrowser();
    }

    ui->textBrowser_3->append(QString::number(block_number));
}

void MainWindow::on_folderBrowser_cursorPositionChanged()
{
    update_file_list();
    unsigned int file_number = ui->folderBrowser->textCursor().blockNumber();
    QTextCursor cur = ui->folderBrowser->textCursor();
    QTextBlockFormat f;
    cur.select(QTextCursor::LineUnderCursor);
    cur.setBlockFormat(f);
    ui->folderBrowser->setTextCursor(cur);

    fileName_full = APPDATA_PATH + "/" +QString::fromStdString(fileList.at(file_number).toStdString());

    file_open_and_read();

    update_textBrowser();

}

void MainWindow::on_addFile_clicked()
{
    fileName_full = APPDATA_PATH + "/" + Utility::random_string(13) + ".txt";

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
    if (fileList.size() <= 1 )     //|| 1 == ui->folderBrowser->textCursor().blockNumber())
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
    ui->folderBrowser->clear();
    ui->folderBrowser->append(fileList.join("\n"));
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
    if(block_number == (all_list_size/10))
    {
        block_number -= 1;
    }
}

void MainWindow::initialize_file_structure()
{
    QDir dir(APPDATA_PATH);
    if (!dir.exists())
        dir.mkdir(APPDATA_PATH);

    fileName_full = APPDATA_PATH + "/default.txt";

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
