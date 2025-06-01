#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <windows.h>
#include "process/ProcessInfo.h"
#include "process/ProcessModel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*! @brief Конструктор класса главного окна */
    MainWindow(QWidget *parent = nullptr);
    /*! @brief Деструктор */
    ~MainWindow();

    
signals:
    /*! @brief Сигнал посылки логируемого сообщения */
    void sendLogMessage(const QString &message);
public slots:
    /*! @brief Слот оброботки списка информации о процессах */
    void sendProcessInfo(QList<ProcessInfo>infProcess);
    /*! @brief Слот формирования всплывающего меню  */
    void slotCustomMenuRequested(QPoint pos);
    /*! @brief Слот удаления процесса */
    void deleteProcess();
    /*! @brief Слот выбора включения показа диаграммы */
    void selectShowChart();

private:
    Ui::MainWindow *ui;

    ProcessModel *mModel;//! Модель для отображения информации о процессах

    QTimer *timer;//!Таймер для отсчета 1 секунды
    QList<ProcessInfo>mProcesses;//!Список информации о процессах
    QList<SIZE_T> memoryUsageList; //! История размеров памяти
    QLineSeries* series;//!Указатель на данные диаграммы
    QChart* chart;//!Указатель на диаграмму
    QChartView* chartView;//!Указатель на отображение диаграммы
    QValueAxis* axisX;//!Указатель на ось X диаграммы
    QValueAxis* axisY;//!Указатель на ось У диаграммы
    QModelIndex twIndex;//!Индекс для выбранной ячейки в таблице
    int indexRow = -1;//!индекс процесса в списке
    int maxPoints = 60;//!количество временны точек
    bool flagShowChart = false;//!Флаг разрещающий выводить данные на график
    DWORD scPID = 0;//!pid процесса для которого будет формироватся график

private:
    /*! @brief Метод создания отображения данных */
    void createView();
    /*! @brief Метод создания логера */
    void createLogger();
    /*! @brief Метод настройки таблицы */
    //void settingTableWidget();
    /*! @brief Метод создания обрабочика информации о процессах */
    void createProcessHandler();
    /*! @brief Метод создания компонентов для диаграммы */
    void createChart();
    /*! @brief Метод настройки диаграммы */
    void setupChart(QWidget* parentWidget);
    /*! @brief Метод обновления набора данных графика */
    void showChart(QList<ProcessInfo> infProcess);
    /*! @brief Метод обновления набора данных */
    void addMemoryUsage(SIZE_T value);

    /*! @brief Метод обновления таблицы */
    //void updateProcessTable(QTableWidget* tableWidget, QList<ProcessInfo> infProcess);

    /*! @brief Метод прерывания процесса */
    bool terminateProcessByPID(DWORD pid);
};
#endif // MAINWINDOW_H
