#ifndef DBWORKER_H
#define DBWORKER_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDomElement>
#include <QtWidgets>
#include <QMessageBox>
#include <QDebug>
#include <QAbstractButton>
#include <QSqlTableModel>
#include "checkboxdelegate.h"
class dbWorker
{
public:
    dbWorker();
    QSqlDatabase db, abiDB;
    QSqlQuery *query;

public:
     void dbConnect(int n, QComboBox *QComboBox, QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions);
     void abiConnect(QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions, std::vector <QDomElement> list);
     void checkResourses(QComboBox *combobx, QAbstractItemDelegate *deleg,  QTableView *table, QString usr, QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions);
     void settingTriggers(QComboBox *cmb, QTableView *tbl, const int &state);
     void loggingDBAccess(int action, QComboBox *cmb, QTableView *tbl);
};

#endif // DBWORKER_H
