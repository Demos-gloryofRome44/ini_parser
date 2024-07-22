#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtWidgets>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QDir>
#include "IniFile.h"
#include <QInputDialog>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_File_triggered();
    void file_clicked_in_list();
    void on_actionSave_File_triggered();
    void update_combo_Box(QTreeWidgetItem *selected_item);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void update_comboBox_5(QTreeWidgetItem *item);

    void on_actionAdd_Directory_triggered();

    void on_pushButton_4_clicked();

    void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    QTreeWidget *treeWidget;
    QListWidget *listWidget;
    QComboBox *comboBox_2;//type
    IniFile *inifile;
    //validator
    QValidator* validatorInt;
    QValidator* validatorDouble;
    QValidator *validatorBool;
};
#endif // MAINWINDOW_H

