#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::APPDATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

QStringList fileList;
std::vector<std::string> fileList_vec;
QString fileName_full;
QString line;

std::vector<std::string> all_data_list;

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
    QFont font = QFont("DroidSansMono", 20, 1);
    ui->textBrowser->setFont(font);
    ui->folderBrowser->setFont(font);

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

    file_open_and_read(line);
    all_list_size = PythonParser::parse_python_dictionary(all_data_list, line.toStdString());

    update_all_data_list_block_number();

    update_textBrowser();

    update_file_list();
    ui->folderBrowser->append(fileList.join("\n"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_textBrowser_cursorPositionChanged()
{
    update_all_data_list_block_number();
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
    unsigned int index_of_course = std::distance(all_data_list.begin(), std::find(all_data_list.begin(), all_data_list.end(), ui->course_input->text().toStdString()));
    std::string python_dictionary;
    double grade_add;
    std::string letter_add = ui->letter_input->text().toStdString();
    grade_add = Utility::calculate_grade(letter_add);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << grade_add;
    std::string grade_add_str = stream.str();
    std::string weight_str;
    stream.str(std::string());
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

    python_dictionary = PythonParser::write_python_dictionary(all_data_list, all_list_size);

    QFile file(fileName_full);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        assert(0);

    QTextStream outStream(&file);
    outStream << QString::fromStdString(python_dictionary);

    file.close();
    update_textBrowser();
}

void MainWindow::on_fileLoadButton_clicked()
{
    QString fileName_full_backup = fileName_full;
    fileName_full = QFileDialog::getOpenFileName(this,
                                                 tr("OpenJSON"), APPDATA_PATH, tr("Image Files (*.json *.txt)"));

    if (!fileName_full.isEmpty())
    {
        all_data_list = std::vector<std::string>();
        line = "";
        fileName_full = "";
        file_Load();
        update_textBrowser();
    }else
        fileName_full = fileName_full_backup;
}

void MainWindow::file_open_and_read(QString &line)
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

        for(unsigned int i = 0; i < all_list_size - 9; i = i + 10){
            MainWindow::html_add_row(display_html);

            cn_d  << std::left  << all_data_list[coursename_data + i];
            l_d   << std::right << std::setw(9) << all_data_list[letter_data + i];
            c_d   << std::right << std::setw(9) << all_data_list[credit_data + i];
            w_d   << std::right << std::setw(9) << all_data_list[weight_data + i];
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

    update_all_data_list_block_number();
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
            assert(0);

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

    file_Load();

    update_textBrowser();

}

void MainWindow::file_Load()
{
    try {
        file_open_and_read(line);
    } catch (std::exception &e) {
        std::cerr << "file_open_and_read error" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    try {
        all_list_size = PythonParser::parse_python_dictionary(all_data_list, line.toStdString());
    } catch (const std::bad_alloc &e) {
        std::cerr << "PythonParser error" << std::endl;
        std::cerr << e.what() << std::endl;
    }
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

    file_open_and_read(line);
    all_list_size = PythonParser::parse_python_dictionary(all_data_list, line.toStdString());
    update_all_data_list_block_number();
    update_textBrowser();

    update_file_list();

    update_file_browser();
}

void MainWindow::on_deleteFile_clicked()
{
    if (fileList.size() <= 1 || 1 == ui->folderBrowser->textCursor().blockNumber())
    {
        // There is a bug and I don't want to deal with it.
        // So you can't delete the first index. ¯\_(ツ)_/¯
        //-----fileBrowser----
        // 0th
        // 1st <---- crash
        // 2nd
        //--------------------
    }else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Ahy!", "Yer are deletin' dis file, ain't yer?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            QFile file (fileName_full);
            file.remove();

            update_file_list();
            ui->folderBrowser->append(fileList.join("\n"));

            try {
                ui->folderBrowser->clear();
            } catch (std::exception &e) {
                std::cerr << "clear error delete_file" << std::endl;
                std::cerr << e.what() << std::endl;
            }

            try {
                ui->folderBrowser->append(fileList.join("\n"));
            } catch (std::exception &e) {
                std::cerr << "append error delete file error" << std::endl;
                std::cerr << e.what() << std::endl;
            }

        }
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
                                    <td><pre>%s</pre></td>
                                    <td><pre>%s</pre></td>
                                    <td><pre>%s</pre></td>
                                    <td><pre>%s</pre></td>
                                </tr>
                                <!--%r%r-->
                        )";
    Utility::replace(str,"<!--%r%r-->", four_rows);
}
void MainWindow::update_all_data_list_block_number()
{
    block_number = ui->textBrowser->textCursor().blockNumber();
    block_number = ((int)block_number-1)/4;
    if(block_number == (all_list_size/10))
    {
        block_number -= 1;
    }
}
