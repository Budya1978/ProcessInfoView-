#ifndef PROCESSINFORMATIONHANDLER_H
#define PROCESSINFORMATIONHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <QString>
#include <QDateTime>

#include "ProcessInfo.h"

class ProcessInformationHandler : public QObject
{
    Q_OBJECT
public:
    explicit ProcessInformationHandler(QObject *parent = nullptr);
    ~ProcessInformationHandler();

signals:
    /*! @brief Сигнал отправки списка с информацией о проекте */
    void sendProcessInfo(QList<ProcessInfo>infProcess);
    /*! @brief Сигнал посылки логируемого сообщения */
    void sendLogMessage(const QString &message);

public slots:
    /*! @brief Слот обработки параметров системы */
    void processing();

private:
    //! Карта процессов
    QMap<DWORD,ProcessInfo>mapProcessInfo;


private:
    /*! @brief Метод возвращает путь к запущеному процессу */
    QString getProcessPath(DWORD pid);
    /*! @brief Метод возвращает объем используемой памяти*/
    SIZE_T getMemoryUsage(HANDLE hProcess);
    /*! @brief Метод возвращает время запуска*/
    QString getStartTime(HANDLE hProcess);


};

#endif // PROCESSINFORMATIONHANDLER_H
