#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    classLogFunction clParam;
        clParam.Read_Log_param();

        qDebug()
            << "Информация по отладке выводится (0- консоль, 1 - файл с логом): "
            << variable_log::instance()->prDebug() << "\n"
            << "Файл лога : " << variable_log::instance()->pathLogDebug() << endl;

        //! 2. Определяем куда должна сохраняться отладочная информация
        if (variable_log::instance()->prDebug() == 1)
            //! \a Если  \link prDebug() \a равна 1, \a записываем  \a в \a файл \a лога, иначе просто выводм в консоль.
            clParam.WriteToLog("shredBase", Module);

    MainWindow w;
    w.setWindowTitle("Настройка протоколирования операций пользователей");
    w.show();

    return a.exec();
}
