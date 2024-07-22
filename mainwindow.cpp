#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "IniFile.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QListWidget>
#include <QTreeWidget>
#include <QList>
#include <QTreeWidgetItem>
#include <map>
#include <string>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    treeWidget = ui->treeWidget;
    listWidget = ui->listWidget;
    QComboBox *comboBox_2 = ui->Group_box->findChild<QComboBox *>();

    validatorInt = new QIntValidator(this);
    validatorDouble = new QDoubleValidator(this);
    validatorBool = new QRegularExpressionValidator(QRegularExpression("^(1|0|true|false|True|False)$"),this);
    comboBox_2->addItem("int");
    comboBox_2->addItem("double");
    comboBox_2->addItem("bool");
    comboBox_2->addItem("string");

    connect(ui->actionAdd_File, &QAction::triggered, this, &MainWindow::on_actionAdd_File_triggered);

    connect(ui->actionAdd_Directory, &QAction::triggered, this, &MainWindow::on_actionAdd_Directory_triggered);

    connect(ui->actionSave_File, &QAction::triggered, this, &MainWindow::on_actionSave_File_triggered);

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::file_clicked_in_list);

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::update_combo_Box);

    connect(ui->comboBox_2, (&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_2_currentIndexChanged);

    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::on_pushButton_5_clicked);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::update_comboBox_5);

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::on_pushButton_4_clicked);
}

int count = 0;
MainWindow::~MainWindow()
{
    delete ui;
    delete validatorInt;
    delete validatorDouble;
    delete validatorBool;
    delete inifile;
}

// открытие файла
void MainWindow::on_actionAdd_File_triggered(){
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "INI Files (*.ini)", 0, QFileDialog::DontUseNativeDialog);

    if (!fileName.isEmpty()) {
        QList<QListWidgetItem *> file = ui->listWidget->findItems(fileName, Qt::MatchWildcard);
        if (!file.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Файл уже выбран");
        } else {
            ui->listWidget->addItem(fileName);
            //inifile = new IniFile(ui->listWidget->currentItem()->text().toStdString());
        }
    }
}

// открытие директории
void MainWindow::on_actionAdd_Directory_triggered() {
    QString dirName = QFileDialog::getExistingDirectory(this, "Открыть директорию", "", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if (!dirName.isEmpty()) {
        QDir dir(dirName);
        QStringList files = dir.entryList(QStringList() << "*.ini", QDir::Files);

        for (const QString& file : files) {
            QString fullPath = dir.absoluteFilePath(file);
            QList<QListWidgetItem *> existingItems = ui->listWidget->findItems(fullPath, Qt::MatchExactly);
            if (existingItems.isEmpty()) {
                ui->listWidget->addItem(new QListWidgetItem(fullPath));
            }
        }
    }
    else {
        QMessageBox::warning(this, "Ошибка", "Директория не выбрана!");
    }
}

// построение дерева в treeWidget
void MainWindow::file_clicked_in_list()
{
    ui->treeWidget->clear();
    QStringList labels = {"Keys", "Values"};
    ui->treeWidget->setHeaderLabels(labels);

    inifile = new IniFile(ui->listWidget->currentItem()->text().toStdString());

    SectionsMap sections = inifile->getSections();
    for(const auto &section : sections){
        QTreeWidgetItem *sectionItem = new QTreeWidgetItem(ui->treeWidget);
        sectionItem->setText(0, QString::fromStdString(section.first));

        for(auto &keyValue : section.second){
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(sectionItem);
            keyItem->setText(0, QString::fromStdString(keyValue.first));
            keyItem->setText(1, QString::fromStdString(keyValue.second));
        }

        sectionItem->setExpanded(true);
    }
    ui->statusbar->showMessage("Open files:" + QString::number(ui->listWidget->count()));
}

// сохраниение файла
void MainWindow::on_actionSave_File_triggered() {
    if (QListWidgetItem* l = ui->listWidget->currentItem()) {
        try {
            inifile->save();
            ui->treeWidget->clear();
            QMessageBox::information(this, "Save", "Вы сохранили файл!", QMessageBox::Ok);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what(), QMessageBox::Ok);
        }
    } else {
        QMessageBox::information(this, "Ошибка", "Вы не выбрали файл!", QMessageBox::Ok);
    }
}

// список секций
void MainWindow::update_combo_Box(QTreeWidgetItem *item)
{
    ui->comboBox->clear();

    std::string file = ui->listWidget->currentItem()->text().toStdString();

    QStringList Sec;
    SectionsMap sections = inifile->getSections();

    for (const auto &item : sections) {
        Sec.append(QString:: fromStdString(item.first));
    }
    if (item->parent() == nullptr) {
        ui->comboBox->insertItems(0, Sec);
        ui->comboBox->setCurrentText(item->text(0));
        return;
    }
    QString section = item->parent()->text(0);
    ui->comboBox->insertItems(0, Sec);
    ui->comboBox->setCurrentText(section.toStdString().c_str());
}

// списко типов
void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    if (index == 0){//int
        ui->lineEdit_2->clear();
        ui->lineEdit_2->setValidator(validatorInt);
    }
    else if (index == 1){//double
        ui->lineEdit_2->clear();
        ui->lineEdit_2->setValidator(validatorDouble);
    }
    else if (index == 2){//bool
        ui->lineEdit_2->clear();
        ui->lineEdit_2->setValidator(validatorBool);
    }
    else{//string
        ui->lineEdit_2->clear();
        ui->lineEdit_2->setValidator(nullptr);
    }
}

// добавление ключа и значения в секцию
void MainWindow::on_pushButton_5_clicked()
{
    QString key = ui->lineEdit->text();
    QString value = ui->lineEdit_2->text();
    if (key.isEmpty() || value.isEmpty()){
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }
    else if (!key.isEmpty() && !value.isEmpty()){
        switch (ui->comboBox_2->currentIndex()){
        case 0:
            inifile->writeInt((ui->comboBox->currentText()).toStdString(), key.toStdString(), value.toInt());
            break;
        case 1:
            inifile->writeDouble((ui->comboBox->currentText()).toStdString(), key.toStdString(), value.toDouble());
            break;
        case 2:
            inifile->writeBool((ui->comboBox->currentText()).toStdString(), key.toStdString(), (value.toInt() != 0));
            break;
        case 3:
            inifile->writeString((ui->comboBox->currentText()).toStdString(), key.toStdString(), value.toStdString());
        }

        SectionsMap sections = inifile->getSections();
        ui->treeWidget->clear();
        for(const auto &section : sections){
            QTreeWidgetItem *sectionItem = new QTreeWidgetItem(ui->treeWidget);
            sectionItem->setText(0, QString::fromStdString(section.first));

            for(auto &keyValue : section.second){
                QTreeWidgetItem *keyItem = new QTreeWidgetItem(sectionItem);
                keyItem->setText(0, QString::fromStdString(keyValue.first));
                keyItem->setText(1, QString::fromStdString(keyValue.second));
            }

            sectionItem->setExpanded(true);
        }
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        return;
    }
}

//добавление секции
void MainWindow::on_pushButton_2_clicked()
{
    QString str = QInputDialog::getText(this, tr("Add Section"), tr("Введите новое имя секции:"));
    if (!str.isEmpty()) {
        std::string newSection = str.toStdString();
        if (inifile->addNewSection(newSection)) {
            QTreeWidgetItem *addSection = new QTreeWidgetItem(ui->treeWidget);
            addSection->setText(0, str);
            ui->treeWidget->addTopLevelItem(addSection);
            ui->comboBox->addItem(str);
        } else {
            QMessageBox::warning(this, "Ошибка", "Секция с таким именем уже существует.");
        }
    }
}

//удаление секции
void MainWindow::on_pushButton_clicked()
{
    QString selected_sec = ui->comboBox->currentText();
    inifile->deleteSection(selected_sec.toStdString());

    int index = ui->comboBox->currentIndex();
    ui->comboBox->removeItem(index);
    ui->comboBox->setCurrentIndex(-1);
    ui->treeWidget->clear();
    SectionsMap sections = inifile->getSections();
    for(const auto &section : sections){
        QTreeWidgetItem *sectionItem = new QTreeWidgetItem(ui->treeWidget);
        sectionItem->setText(0, QString::fromStdString(section.first));

        for(auto &keyValue : section.second){
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(sectionItem);
            keyItem->setText(0, QString::fromStdString(keyValue.first));
            keyItem->setText(1, QString::fromStdString(keyValue.second));
        }

        sectionItem->setExpanded(true);
    }
}

// список ключей из секции
void MainWindow::update_comboBox_5(QTreeWidgetItem *item)
{
    ui->comboBox_5->clear();

    std::string file = ui->listWidget->currentItem()->text().toStdString();

    QStringList Keys;
    SectionsMap sections = inifile->getSections();

    for (const auto &item : sections) {
        if (item.first == (ui->comboBox->currentText()).toStdString()) {
            for (const auto &key : item.second) {
                Keys.append(QString::fromStdString(key.first));
            }
        }
    }
    QString key = item->text(0);
    ui->comboBox_5->insertItems(0, Keys);
    ui->comboBox_5->setCurrentText(key.toStdString().c_str());
}

//удаление ключа из секции
void MainWindow::on_pushButton_4_clicked()
{
    QString selected_sec = ui->comboBox->currentText();
    QString selected_key = ui->comboBox_5->currentText();

    inifile->deleteKey(selected_sec.toStdString(), selected_key.toStdString());
    SectionsMap sections = inifile->getSections();

    ui->treeWidget->clear();
    for(const auto &section : sections){
        QTreeWidgetItem *sectionItem = new QTreeWidgetItem(ui->treeWidget);
        sectionItem->setText(0, QString::fromStdString(section.first));

        for(auto &keyValue : section.second){
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(sectionItem);
            keyItem->setText(0, QString::fromStdString(keyValue.first));
            keyItem->setText(1, QString::fromStdString(keyValue.second));
        }
        sectionItem->setExpanded(true);
    }
}

// создание файла
void MainWindow::on_actionSettings_triggered()
{
    bool ok;
    QString name_file = QInputDialog::getText(this, tr("Create File"), tr("Введите новое имя файла:"), QLineEdit::Normal, "", &ok);

    if (ok && !name_file.isEmpty()) {
        QFile file(name_file);
        if (file.open(QIODevice::WriteOnly)) {
            ui->listWidget->addItem(name_file);
            QMessageBox::information(nullptr, "Успех", "Файл " + name_file + " был успешно создан.");
        } else {
            QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать файл " + name_file + ".");
        }
    } else {
        QMessageBox::warning(nullptr, "Предупреждение", "Вы не ввели название файла!");
    }
}



