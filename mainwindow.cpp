#include "mainwindow.h"
#include "ui_mainwindow.h"

QStringList fileList;
std::vector<std::string> fileList_vec;
QString fileName_full;
QString line;

std::vector<std::string> all_original;
std::vector<std::string> all_display;

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

    QDir dir(m_path);

    if (!dir.exists())
        dir.mkdir(m_path);

    fileName_full = m_path + "/default.txt";

    if(!fileExists(fileName_full))
    {
        QFile file(fileName_full);
        try {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
        } catch (std::exception &e) {
            std::cout << "can't create a default file" << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }
        QTextStream outStream(&file);
        outStream << QString::fromStdString(default_py_dic);
        file.close();
    }

    file_open_and_read(line);
    all_list_size = PythonParser::parse_python_dictionary(all_original, line.toStdString());
    all_display = all_original;


    block_number = ui->textBrowser->textCursor().blockNumber();
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
    block_number = ui->textBrowser->textCursor().blockNumber();
    block_number = ((int)block_number-1)/4;

    if(block_number == (all_list_size/10))
    {
        block_number -= 1;
    }

    std::cout << (block_number) << std::endl;
    std::cout << ((all_list_size/10)) << std::endl;
    QTextCursor cur = ui->textBrowser->textCursor();
    QTextBlockFormat f;
    //QString text;
    //f.setBackground(Qt::red);
    cur.select(QTextCursor::LineUnderCursor);
    cur.setBlockFormat(f);
    ui->textBrowser->setTextCursor(cur);
    //text = cur.selectedText();

    ui->course_input->setText(QString::fromStdString(all_original[coursename_data + (block_number*10)]));
    ui->letter_input->setText(QString::fromStdString(all_original[letter_data + (block_number*10)]));
    ui->credit_input->setText(QString::fromStdString(all_original[credit_data + (block_number*10)]));
}

void MainWindow::on_addButton_clicked()
{
    unsigned int index_of_course = std::distance(all_original.begin(), std::find(all_original.begin(), all_original.end(), ui->course_input->text().toStdString()));
    std::string python_dictionary;
    double grade_add;
    std::string letter_add = ui->letter_input->text().toStdString();
    grade_add = calculate_grade(letter_add);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << grade_add;
    std::string grade_add_str = stream.str();
    std::string weight_str;
    stream.str(std::string());
    stream << std::fixed << std::setprecision(1) << ((ui->credit_input->text().toDouble())*(grade_add));
    weight_str = stream.str();
    float temp = ((all_list_size + 0.5 )/10);
    all_list_size = 10 * floor(temp);

    if (index_of_course >= all_original.size()){
        //std::cout << "not here" << std::endl;
        if (all_original.size() <= all_list_size+10)
            all_original.resize(all_list_size+11);
        all_original[all_list_size+coursename] = "courseName";
        all_original[all_list_size+coursename_data] = ui->course_input->text().toStdString();
        all_original[all_list_size+credit] =  "credit";
        all_original[all_list_size+credit_data] = ui->credit_input->text().toStdString();
        all_original[all_list_size+letter] =  "letter";
        all_original[all_list_size+letter_data] = ui->letter_input->text().toStdString();
        all_original[all_list_size+grade] =  "grade";
        all_original[all_list_size+grade_data] = grade_add_str;
        all_original[all_list_size+weight] =  "weight";
        all_original[all_list_size+weight_data] = weight_str;
        all_list_size = all_list_size + 10;
    }
    else {

        all_original[coursename_data + (block_number*10)] =  ui->course_input->text().toStdString();
        all_original[letter_data + (block_number*10)] = ui->letter_input->text().toStdString();
        all_original[credit_data + (block_number*10)] = ui->credit_input->text().toStdString();
        all_original[grade_data + (block_number*10)] = grade_add_str;
        all_original[weight_data + (block_number*10)] = weight_str;

        //std::cout << "yes here" << std::endl;
    }

    all_display = all_original;
    python_dictionary = PythonParser::write_python_dictionary(all_original, all_list_size);

    QFile file(fileName_full);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        assert(0);

    QTextStream outStream(&file);
    outStream << QString::fromStdString(python_dictionary);
    std::cout<<python_dictionary<<std::endl;

    file.close();
    update_textBrowser();
}

void MainWindow::on_fileLoadButton_clicked()
{
    QString fileName_full_backup = fileName_full;
    fileName_full = QFileDialog::getOpenFileName(this,
                                                 tr("OpenJSON"), m_path, tr("Image Files (*.json *.txt)"));

    if (!fileName_full.isEmpty())
    {
        all_original = std::vector<std::string>();
        all_display = std::vector<std::string>();
        line = "";
        fileName_full = "";
        file_Load();
        update_textBrowser();
    }else
        fileName_full = fileName_full_backup;
}

void MainWindow::file_open_and_read(QString &line)
{

    std::cout << m_path.toStdString() << std::endl;

    QFile file(fileName_full);

    try {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    } catch (std::exception &e) {
        std::cout << "can't read file" << std::endl;
        std::cerr << e.what() << std::endl;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    file.close();
}

bool MainWindow::fileExists(QString m_path)
{
    QFileInfo check_file(m_path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
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

        for(unsigned int i = 0; i < all_list_size - 9; i = i + 10){
            html_add_row(display_html);

            cn_d  << std::left  << all_display[coursename_data + i];
            l_d   << std::right << std::setw(9) << all_display[letter_data + i];
            c_d   << std::right << std::setw(9) << all_display[credit_data + i];
            w_d   << std::right << std::setw(9) << all_display[weight_data + i];
            replace(display_html, "%s", cn_d.str());
            replace(display_html, "%s", l_d.str());
            replace(display_html, "%s", c_d.str());
            replace(display_html, "%s", w_d.str());
            cn_d.str("");
            l_d.str("");
            c_d.str("");
            w_d.str("");
        }
        ui->textBrowser->setHtml(QString::fromStdString(display_html));
        //std::cout << display_html <<std::endl;
    } catch (std::exception &e) {
        std::cout << "can't update textBrowser" << std::endl;
        std::cerr << e.what() << std::endl;
        return;
    }


}
double MainWindow::calculate_grade(std::string letter_add)
{
    unsigned int index;
    double grade_add;

    index = std::distance(m_validLetters.begin(), std::find(m_validLetters.begin(), m_validLetters.end(), letter_add));

    if (index>=8 and index<=13)
        grade_add = 0;
    else if (index>= 14 and index<=15)
        grade_add = 4;
    else
        grade_add =  (4 - 0.5*index);
    return grade_add;
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
        std::rotate(all_original.begin()+index, all_original.begin()+index+delete_size, all_original.end());
        all_original.resize(all_original.size()-10);
        all_display = all_original;

        std::string python_dictionary;

        python_dictionary = PythonParser::write_python_dictionary(all_original, all_list_size);

        QFile file(fileName_full);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            assert(0);

        QTextStream outStream(&file);
        outStream << QString::fromStdString(python_dictionary);
        std::cout<<python_dictionary<<std::endl;

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

    std::cout << fileList.at(file_number).toStdString() << std::endl;
    fileName_full = m_path + "/" +QString::fromStdString(fileList.at(file_number).toStdString());
    std::cout << fileName_full.toStdString()<< std::endl;

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
        all_list_size = PythonParser::parse_python_dictionary(all_original, line.toStdString());
    } catch (const std::bad_alloc &e) {
        std::cerr << "PythonParser error" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    all_display = all_original;
}

void MainWindow::on_addFile_clicked()
{
    fileName_full = m_path + "/" + random_string(13) + ".txt";
    std::cout << fileName_full.toStdString() << std::endl;

    if(!fileExists(fileName_full))
    {
        QFile file(fileName_full);
        try {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
        } catch (std::exception &e) {
            std::cout << "can't create a default file" << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }
        QTextStream outStream(&file);
        outStream << QString::fromStdString(default_py_dic);
        file.close();
    }

    file_open_and_read(line);
    all_list_size = PythonParser::parse_python_dictionary(all_original, line.toStdString());
    all_display = all_original;
    block_number = ui->textBrowser->textCursor().blockNumber();
    update_textBrowser();

    update_file_list();
    std::cout << fileList.join("\n").toStdString() << std::endl;

    update_file_browser();
}

void MainWindow::on_deleteFile_clicked()
{
    std::cout << fileList.size()<<std::endl;

    if (fileList.size() <= 1 || 1 == ui->folderBrowser->textCursor().blockNumber())
    {
        // There is a bug and I don't want to deal with it.
        // So you can't delete the first index. ¯\_(ツ)_/¯
        // 0th
        // 1st <---- crash
        // 2nd
    }else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Ahy!", "Yer are deletin' dis file, ain't yer?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            std::cout << "Yes was clicked";
            std::cout << fileName_full.toStdString() <<std::endl;
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

        } else {
            std::cout << "Yes was *not* clicked";
        }
    }
}

void MainWindow::update_file_list(){
    QDir export_folder(m_path);
    export_folder.setNameFilters(QStringList()<<"*.txt"<<"*.json");
    fileList = export_folder.entryList();
    //ui->folderBrowser->append(fileList.join("\n"));
}

QString MainWindow::random_string(int length = 13)
{
    QString allow_symbols=QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    QString result;
    qsrand(QTime::currentTime().msec());
    for (int i = 0; i < length; ++i) {
        result.append(allow_symbols.at(qrand() % (allow_symbols.length())));
    }
    return result;
}

void MainWindow::update_file_browser(){
    ui->folderBrowser->clear();
    ui->folderBrowser->append(fileList.join("\n"));
}

bool MainWindow::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void MainWindow::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
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
    replace(str,"<!--%r%r-->", four_rows);
}
