#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    xml = new XmlParser();
    dbwork = new dbWorker();

    createUI();

    if (xml->open(PATH_DATABASE)) {          ///< Открываем database.xml, заносим в std::vector элементы из файла. К данным можно будет обращаться по определённому тегу при помощи attributes. Заполняем comboBox именами баз данных.
        qDebug() << "[Information] Идёт чтение database.xml... Данные успешно получены!";
            dbworker = xml->ListElements(xml->getRoot(), "database");
            for (unsigned int n = 0; n < dbworker.size(); n++){
                comboBox->addItem(dbworker[n].attribute("name"),QVariant(n));
            }
            dbwork->abiConnect (dbworker[0].attribute(("driver"),"abi"),
                                dbworker[0].attribute("hostname"),
                                dbworker[0].attribute("port").toInt(),
                                dbworker[0].attribute("name"),
                                "lostdox1",
                                "postgres",
                                dbworker[0].attribute("options"),
                                dbworker);
    }else
    {
        QMessageBox::critical(0, "Ошибка открытия файла", "Файл не открыт! Проверьте права доступа database.xml, убедитесь\nчто файл расположен по пути /mnt/settings/,\nа также проверьте правильность содержимого xml файла. Закрытие приложения!");
        qDebug() << "[Error] Файл не открыт! Проверьте права доступа database.xml, убедитесь\nчто файл расположен по пути /mnt/settings/,\nа также проверьте правильность содержимого xml файла."; ///< Если файл не открыт - пользователю показывается всплывающее окно с ошибкой.
        QApplication::exit();
    }

    connect(button, &QPushButton::clicked, this, [this]
    {
        int i = comboBox->currentIndex();
        dbwork->checkResourses (comboBox,
                                delegate,
                                table1,
                                usr,
                                dbworker[i].attribute("driver"),
                                dbworker[i].attribute("hostname"),
                                dbworker[i].attribute("port").toInt(),
                                dbworker[i].attribute("name"),
                                "lostdox1",
                                "postgres",
                                dbworker[i].attribute("options"));
    });

    connect(delegate, &checkboxDelegate::_change, [this](const int& state) ///< Слот, обеспечивающий логирование в БД abi_db, а также включение/выключение триггерных функций.
    {
        dbwork->settingTriggers(comboBox,table1,(bool)state);
    });

    connect(buttoncls, &QPushButton::clicked,this, []
    {
       QApplication::exit();
    });
}



void MainWindow::createUI()
{
    form =  new QWidget;  ///< В самом начале создаются формы программы.
    hightlayout = new QHBoxLayout;
    lowlayout = new QHBoxLayout;
    vlayout = new QVBoxLayout;
    mainLayout = new QVBoxLayout;

    comboBox =  new QComboBox;
    table1   =  new QTableView;
    button   =  new QPushButton("Посмотреть настройки таблиц");
    buttoncls = new QPushButton("Закрыть");
    delegate =  new checkboxDelegate;
    labeldb  =  new QLabel ("База данных:");
    //..End create forms...

    table1->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table1->horizontalHeader()->setStretchLastSection(true);
    table1->horizontalHeader    ()->setStretchLastSection(true);
    table1->setSelectionMode    (QAbstractItemView::SingleSelection); /// < Устанавливаем в tableview одиночную выборку строк.
    table1->setEditTriggers(QAbstractItemView::NoEditTriggers); /// < Устанавливаем запрет на редактирование ячеек.


    form->setObjectName         ("form");
    form->setMinimumSize        (1050,650);
    setStyleSheet               ("QWidget#form{background: white;}");

    labeldb->setMaximumSize(78,78);

    hightlayout->addWidget(labeldb); hightlayout->addWidget(comboBox);  hightlayout->addWidget(button); hightlayout->addStretch();
    vlayout->addWidget(table1);
    lowlayout->addStretch();
    lowlayout->addWidget(buttoncls);
    mainLayout->addLayout(hightlayout); mainLayout->addLayout(vlayout); mainLayout->addLayout(lowlayout);
    form->setLayout(mainLayout);
    setCentralWidget(form);
}

MainWindow::~MainWindow()
{
    delete ui;
}
