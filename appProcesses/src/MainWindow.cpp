#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMenu>
#include <QThread>
#include <QTimer>
#include "process/Logger.h"
#include "process/ProcessInformationHandler.h"


//*************************************************************
/*!
 * \brief MainWindow::MainWindow Конструктор класса главного окна
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createView();
    createLogger();
    createProcessHandler();
    createChart();
    setupChart(ui->chartWidget);
}

//*************************************************************
/*!
 * \brief MainWindow::~MainWindow Деструктор
 */
MainWindow::~MainWindow()
{
    timer->stop();
    delete timer;
    delete series;
    delete chart;
    delete chartView;
    delete axisX;
    delete axisY;
    delete mModel;
    delete ui;
}

//*************************************************************
/*!
 * \brief MainWindow::createView
 */
void MainWindow::createView()
{
    mModel = new ProcessModel(this);
    ui->tableView->setModel(mModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    QList<int>listSizeRow = {60, 150, 400, 100, 100, 150};
    for(int i=0;i<listSizeRow.count();i++){
        ui->tableView->setColumnWidth(i,listSizeRow[i]);
    }
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::slotCustomMenuRequested);
}

//*************************************************************
/*!
 * \brief MainWindow::createLogger Метод  создания логера
 */
void MainWindow::createLogger()
{
    QThread *logThread = new QThread;
    Logger *logger = new Logger();
    logger->moveToThread(logThread);
    connect(this, &MainWindow::sendLogMessage,logger, &Logger::logMessage);
    connect(qApp, &QCoreApplication::aboutToQuit, logThread, &QThread::quit);
    connect(logThread, &QThread::finished, logger, &QObject::deleteLater);
    connect(logThread, &QThread::finished, logThread, &QObject::deleteLater);
    logThread->start();
    logger->logMessage("Старт программы");
}

//*************************************************************
/*!
 * \brief MainWindow::createProcessHandler Метод чтения и создания обрабочика информации о процессах
 */
void MainWindow::createProcessHandler()
{
    timer = new QTimer(this);
    ProcessInformationHandler *handler = new ProcessInformationHandler();
    QThread* thread = new QThread;
    handler->moveToThread(thread);
    connect(handler, &ProcessInformationHandler::sendProcessInfo, this, &MainWindow::sendProcessInfo, Qt::QueuedConnection);
    connect(timer, &QTimer::timeout, handler, &ProcessInformationHandler::processing, Qt::QueuedConnection);
    connect(handler, &ProcessInformationHandler::sendLogMessage, this, &MainWindow::sendLogMessage);

    connect(qApp, &QCoreApplication::aboutToQuit, thread, &QThread::quit);
    connect(thread, &QThread::finished, handler, &ProcessInformationHandler::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    timer->start(1000);
}

//*************************************************************
/*!
 * \brief MainWindow::createChart   Метод создания компонентов для диаграммы
 */
void MainWindow::createChart()
{
    series = new QLineSeries();
    chart = new QChart();
    chartView = new QChartView(chart);
    axisX = new QValueAxis();
    axisY = new QValueAxis();
}

//*************************************************************
/*!
 * \brief MainWindow::setupChart Метод настройки диаграммы
 * \param parentWidget
 */
void MainWindow::setupChart(QWidget *parentWidget)
{
    chart->legend()->hide();
    chart->addSeries(series);

    axisX->setLabelFormat("%ds");
    axisX->setTitleText("Время (секунды назад)");
    axisX->setRange(-59, 0);

    axisY->setTitleText("Память (байты)");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(parentWidget);
    chartView->resize(parentWidget->size());
}

//*************************************************************
/*!
 * \brief MainWindow::sendProcessInfo Слот оброботки списка информации о процессах
 * \param infProcess
 */
void MainWindow::sendProcessInfo(QList<ProcessInfo>infProcess)
{
    mProcesses = std::move(infProcess);
    if(flagShowChart){
        showChart(mProcesses);
    }
    mModel->updateProcessList(mProcesses);
    //updateProcessTable(ui->tableWidget,mProcesses);
}

//*************************************************************
/*!
 * \brief MainWindow::slotCustomMenuRequested    Слот формирования всплывающего меню
 * \param pos
 */
void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    twIndex = ui->tableView->currentIndex();
    QMenu * menu = new QMenu(this);
    QAction * actDelProcess = new QAction(tr("Удалить процесс"), this);
    QAction * actShowChart = new QAction(tr("Показать график для процесса"), this);
    connect(actDelProcess, &QAction::triggered, this, &MainWindow::deleteProcess);
    connect(actShowChart, &QAction::triggered, this, &MainWindow::selectShowChart);
    menu->addAction(actShowChart);
    menu->addAction(actDelProcess);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

//*************************************************************
/*!
 * \brief MainWindow::deleteProcess Слот удаления процесса
 */
void MainWindow::deleteProcess()
{
    if(twIndex.isValid()){
        if(twIndex.row() < mProcesses.count()){
            terminateProcessByPID(mProcesses[twIndex.row()].mPid);
        }
    }
}

//*************************************************************
/*!
 * \brief MainWindow::selectShowChart Слот выбора включения показа диаграммы
 */
void MainWindow::selectShowChart()
{
    if(twIndex.isValid()){
        if(twIndex.row() < mProcesses.count()){
            scPID = mProcesses[twIndex.row()].mPid;
            axisY->setTitleText("Память (килобайты) для процесса " + mProcesses[twIndex.row()].getTextValue(1) + " pid:" + mProcesses[twIndex.row()].getTextValue(0));
            series->clear();
            flagShowChart = true;
        }
    }
}

//***********************************************************************************
/*!
 * \brief MainWindow::showChart Метод управления данными для отображения диаграммы
 * \param infProcess
 */
void MainWindow::showChart(QList<ProcessInfo>infProcess)
{
    if(scPID >= 0){
        foreach (ProcessInfo proc, infProcess) {
            if(scPID == proc.mPid){
                addMemoryUsage(proc.mMemoryUsage);
                return;
            }
        }
    }
    flagShowChart = false;
    series->clear();
    axisY->setTitleText("Память (байты)");
}

//***********************************************************************************
/*!
 * \brief MainWindow::addMemoryUsage Метод обновления набора данных
 * \param value новое значение размера памяти
 */
void MainWindow::addMemoryUsage(SIZE_T value)
{
    if (memoryUsageList.size() >= maxPoints)
        memoryUsageList.removeFirst();
    memoryUsageList.append(value);

    series->clear();
    for (int i = 0; i < memoryUsageList.size(); ++i) {
        int x = i - memoryUsageList.size() + 1; // секунд назад
        series->append(x, static_cast<qreal>(memoryUsageList[i]));
    }
    SIZE_T maxMem = *std::max_element(memoryUsageList.begin(), memoryUsageList.end());
    axisY->setRange(0, maxMem * 1.1);
}

//***********************************************************************************
/*!
 * \brief MainWindow::terminateProcessByPID Метод прерывания процесса
 * \param pid прерываемого процесса
 * \return
 */
bool MainWindow::terminateProcessByPID(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        emit sendLogMessage("Не удалось открыть процесс. Ошибка: " + QString().arg(GetLastError()));
        return false;
    }

    // Завершаем процесс
    BOOL result = TerminateProcess(hProcess, 0);
    if (!result) {
        emit sendLogMessage("Не удалось завершить процесс. Ошибка: " + QString().arg(GetLastError()));
        CloseHandle(hProcess);
        return false;
    }

    emit sendLogMessage(QString("Процесс с PID %1 успешно завершён.").arg(pid));
    CloseHandle(hProcess);
    return true;
}

