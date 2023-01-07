#include "dbworker.h"

dbWorker::dbWorker()
{

}

void dbWorker::dbConnect(int n, QComboBox *QComboBox, QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions)
{
    db = QSqlDatabase::addDatabase                  (driverName);
    db.setHostName                                  (hostName);
    db.setPort                                      (port);
    db.setDatabaseName                              (dbName);
    db.setPassword                                  (pwd);
    db.setUserName                                  (userName);
    db.setConnectOptions                            (connectOptions);
    if(db.open()){
        qDebug() << "[Success] Подключение к БД " + dbName + " произошло успешно!";
    }else{
        qDebug() << "[Success] Подключение к БД " + dbName + " произошло с провалом!";
    }
}

void dbWorker::abiConnect(QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions)
{
    abiDB = QSqlDatabase::addDatabase                  (driverName,"abi");  ///< Подключаемся к БД abi_db для логирования действий пользователей.
    abiDB.setHostName                                  (hostName);
    abiDB.setPort                                      (port);
    abiDB.setDatabaseName                              (dbName);
    abiDB.setPassword                                  (pwd);
    abiDB.setUserName                                  (userName);
    abiDB.setConnectOptions                            (connectOptions);
    if(abiDB.open())
    {
       qDebug() << "[Success] Подключение к abi_db произошло успешно. Данные будут логироваться в events_controllaccess!";
    }else
    {
       qDebug() << "[Error] Ошибка подключения к abi_db. Логирование в таблицу events_controllaccess невозможно!";
    }
}

void dbWorker::checkResourses(QComboBox *combobx, QAbstractItemDelegate *deleg, QTableView *table,QString usr,  QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions)
{
    QSqlTableModel *model1;
    QString dbname = combobx->currentText();

    db = QSqlDatabase::addDatabase                  (driverName);
    db.setHostName                                  (hostName);
    db.setPort                                      (port);
    db.setDatabaseName                              (dbName);
    db.setPassword                                  (pwd);
    db.setUserName                                  (userName);
    db.setConnectOptions                            (connectOptions);
    db.open();

    model1 =new QSqlTableModel      (0, db);
    model1->setEditStrategy                         (QSqlTableModel::OnFieldChange);

    model1->setTable                                ("chk.\"Table_state\"");
    model1->setHeaderData                           (0,Qt::Horizontal, "Описание");
    model1->setHeaderData                           (1,Qt::Horizontal, "Имя схемы");
    model1->setHeaderData                           (2,Qt::Horizontal, "Наименование таблицы");
    model1->setHeaderData                           (3,Qt::Horizontal, "Событие вставки");
    model1->setHeaderData                           (4,Qt::Horizontal, "Событие обновления");
    model1->setHeaderData                           (5,Qt::Horizontal, "Событие удаления");
    model1->select                                  ();

    table->setModel                            (model1);
    table->setItemDelegateForColumn            (3, deleg);
    table->setItemDelegateForColumn            (4, deleg);
    table->setItemDelegateForColumn            (5, deleg);
    table->setFocus();


   //фикс видимости чекбоксов
    for(int row=0; row < model1->rowCount(); ++row)
    {
        table->openPersistentEditor        (model1->index(row, 3));
        table->openPersistentEditor        (model1->index(row, 4));
        table->openPersistentEditor        (model1->index(row, 5));
    }

    if(db.isOpen()) ///< Проверяем события успешного подключения к БД
       {
          qDebug() << "[Information] Подключение к БД... \n[Success] Успешное подключение. Пользователь: " + usr + ". Подключение к БД: " + dbname + " IP: "+ hostName;
       }else{
          QMessageBox::critical(0, "Подключение к БД " + dbname, "Ошибка! Пожалуйста, проверьте правильность данных: /mnt/abi/settings/database.xml");
          qDebug() << "[Information] Подключение к БД... \n[Error] Ошибка подключения к БД. Пользователь: " + usr + ". Подключение к БД: " + dbname + " IP: "+ hostName;
       }

    if(table->model()->rowCount() == 0 && db.isOpen()) ///< Проверяем событие наличия таблицы Table_state и её пустоту.
    {
        QMessageBox::warning(0, "Отображение таблицы", "Таблица chk.Table_state либо пуста, либо не создана в БД " + dbname);
    }
}

void dbWorker::settingTriggers(QComboBox *cmb, QTableView *tbl, const int& state)
{
    query = new QSqlQuery(db);
    bool states         = (bool)state;
    int columns         = tbl->selectionModel()->currentIndex().column();

    QString schema      = tbl->selectionModel()->currentIndex().siblingAtColumn(1).data().toString();
    QString tablename   = tbl->selectionModel()->currentIndex().siblingAtColumn(2).data().toString();

    if(tbl->selectionModel()->currentIndex().row()+1 && columns == 3 && states == true) ///< Проверка состояния столбца insert
    {
        QString strInsert = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " ENABLE TRIGGER journal_i;";
        if(query->exec(strInsert)){
            loggingDBAccess(1, cmb, tbl);
            qDebug() << "[Success] Триггер insert для таблицы " + schema +"." + '"' + tablename + '"' + " включён! Запрос: " + strInsert;}
        else{
            QMessageBox::critical(0,"Ошибка", "Включение события вставки в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
            qDebug() << "[Error] Ошибка включения триггера insert для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strInsert;}

    }else if(tbl->selectionModel()->currentIndex().row()+1 && columns == 3 && states == false)
    {
        QString strInsert = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_i;";
        if(query->exec(strInsert)){
            loggingDBAccess(2, cmb, tbl);
            qDebug() << "[Success] Триггер insert для таблицы " + schema +"." + '"' + tablename + '"' + "выключен! Запрос: " + strInsert;}
        else{
            QMessageBox::critical(0,"Ошибка", "Выключение события вставки в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
            qDebug() << "[Error] Ошибка выключения триггера insert для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strInsert;}
    }

    if(tbl->selectionModel()->currentIndex().row()+1 && columns == 4 && states == true) ///< Проверка состояния столбца update
    {
       QString strUpdate = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " ENABLE TRIGGER journal_u;";
       if(query->exec(strUpdate)){
            loggingDBAccess(3, cmb, tbl);
           qDebug() << "[Success] Триггер update для таблицы " + schema +"." + '"' + tablename + '"' + "включен! Запрос: " + strUpdate;}
       else{
           QMessageBox::critical(0,"Ошибка", "Включение события обновления в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
           qDebug() << "[Error] Ошибка включения триггера update для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strUpdate;}

    }else if(tbl->selectionModel()->currentIndex().row()+1 && columns == 4 && states == false)
    {
        QString strUpdate = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_u;";
        if(query->exec(strUpdate)){
            loggingDBAccess(4, cmb, tbl);
            qDebug() << "[Success] Триггер update для таблицы " + schema +"." + '"' + tablename + '"' + "выключен! Запрос: " + strUpdate;}
        else{
            QMessageBox::critical(0,"Ошибка", "Выключение события обновления в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
            qDebug() << "[Error] Ошибка выключения триггера update для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strUpdate;}
    }

    if(tbl->selectionModel()->currentIndex().row()+1 && columns == 5 && states == true) ///< Проверка состояния столбца delete
    {
        QString strDelete = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " ENABLE TRIGGER journal_d;";
        if(query->exec(strDelete)){
            loggingDBAccess(5, cmb, tbl);
            qDebug() << "[Success] Триггер delete для таблицы " + schema +"." + '"' + tablename + '"' + "включен! Запрос: " + strDelete;}
        else{
            QMessageBox::critical(0,"Ошибка", "Включение события удаления в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
            qDebug() << "[Error] Ошибка включения триггера delete для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strDelete;}




    }else if(tbl->selectionModel()->currentIndex().row()+1 && columns == 5 && states == false)
    {
        QString strDelete = "ALTER TABLE "  + schema +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_d;";
        if(query->exec(strDelete)){
            loggingDBAccess(6, cmb, tbl);
        qDebug() << "[Success] Триггер delete для таблицы " + schema +"." + '"' + tablename + '"' + "выключен! Запрос: " + strDelete;}
        else{
            QMessageBox::critical(0,"Ошибка", "Выключение события удаления в " + schema +"." + '"' + tablename + '"' + " произошло с ошибкой!");
            qDebug() << "[Error] Ошибка выключения триггера delete для таблицы " + schema +"." + '"' + tablename + '"' + ". Запрос: " + strDelete;}


    }
}

void dbWorker::loggingDBAccess(int action, QComboBox *cmb, QTableView *tbl)
{
    QString basename = cmb->currentText();
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzzzzz");
    QString user = QDir::home().dirName();
    QString description = tbl->selectionModel()->currentIndex().siblingAtColumn(0).data().toString();
    QString schema      = tbl->selectionModel()->currentIndex().siblingAtColumn(1).data().toString();
    QString tablename   = tbl->selectionModel()->currentIndex().siblingAtColumn(2).data().toString();

    query = new QSqlQuery(abiDB);
    switch (action){
    case 1:
                    query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                    query->bindValue(":blogin", user);
                    query->bindValue(":btable", schema + "."+tablename);
                    query->bindValue(":binfo", "Событие добавления контролируется");
                    query->bindValue(":bdatetime", timestamp);
                    query->bindValue(":bnamebase", basename);
                    query->bindValue(":bdescription", description);
                    if(!query->exec()){
                        qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                    }
        break;
    case 2:
                    query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                    query->bindValue(":blogin", user);
                    query->bindValue(":btable", schema + "."+tablename);
                    query->bindValue(":binfo", "Событие добавления не контролируется");
                    query->bindValue(":bdatetime", timestamp);
                    query->bindValue(":bnamebase", basename);
                    query->bindValue(":bdescription", description);
                    if(!query->exec()){
                        qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                    }
        break;
    case 3:
                   query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                   query->bindValue(":blogin", user);
                   query->bindValue(":btable", schema + "."+tablename);
                   query->bindValue(":binfo", "Событие обновления контролируется");
                   query->bindValue(":bdatetime", timestamp);
                   query->bindValue(":bnamebase", basename);
                   query->bindValue(":bdescription", description);
                   if(!query->exec()){
                       qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                   }
        break;
    case 4:
                    query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                    query->bindValue(":blogin", user);
                    query->bindValue(":btable", schema + "."+tablename);
                    query->bindValue(":binfo", "Событие обновления не контролируется");
                    query->bindValue(":bdatetime", timestamp);
                    query->bindValue(":bnamebase", basename);
                    query->bindValue(":bdescription", description);
                    if(!query->exec()){
                        qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                    }
        break;
    case 5:
                    query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                    query->bindValue(":blogin", user);
                    query->bindValue(":btable", schema + "."+tablename);
                    query->bindValue(":binfo", "Событие удаления контролируется");
                    query->bindValue(":bdatetime", timestamp);
                    query->bindValue(":bnamebase", basename);
                    query->bindValue(":bdescription", description);
                    if(!query->exec()){
                        qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                    }
        break;
    case 6:
                query->prepare("INSERT INTO _abi.events_controllaccess (login, name_table, info, data_time, name_base, description) VALUES (:blogin, :btable, :binfo, :bdatetime, :bnamebase, :bdescription);");
                query->bindValue(":blogin", user);
                query->bindValue(":btable", schema + "."+tablename);
                query->bindValue(":binfo", "Событие удаления не контролируется");
                query->bindValue(":bdatetime", timestamp);
                query->bindValue(":bnamebase", basename);
                query->bindValue(":bdescription", description);
                if(!query->exec()){
                    qDebug() << "[Success] Ошибка логирования в таблицу events_controlaccess!";
                }
        break;
    default:
        break;
    }
}
