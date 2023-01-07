#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "checkboxdelegate.h"
#include "xmlparser.h"
#include "dbworker.h"
#include "/home/liora/ControlProg/common_module/module_log/log_function.h"
#include "/home/liora/ControlProg/common_module/module_log/variable_log.h"
#include "/home/liora/ControlProg/common_module/declaration_path.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    dbWorker *dbwork;
    XmlParser *xml;
    std::vector <QDomElement> dbworker;
    QString usr = QDir::home().dirName();
public: //functions
    void createUI();


public: //forms
    QWidget *form;
    QComboBox *comboBox;
    QTableView *table1;
    QPushButton *button;
    QPushButton *buttoncls;
    checkboxDelegate *delegate;
    QLabel *labeldb;
    QHBoxLayout *hightlayout;
    QHBoxLayout *lowlayout;
    QVBoxLayout *vlayout;
    QVBoxLayout *mainLayout;


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
