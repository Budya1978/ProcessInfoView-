#include "Logger.h"
#include <QDateTime>
#include <QDir>
//******************************************************************************************
/*!
 * \brief Logger::Logger Конструктор класса ведущий лог сообщений
 * \param parent
 */
Logger::Logger(QObject *parent)
    : QObject{parent}, logFile("app_process.log")
{
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning("Не удалось открыть лог-файл для записи.");
    } else {
        stream.setDevice(&logFile);
    }
}

//******************************************************************************************
/*!
 * \brief Logger::~Logger Деструктор
 */
Logger::~Logger()
{
    logMessage("Закрытие приложения");
    logFile.close();
}

//******************************************************************************************
/*!
 * \brief Logger::logMessage Слот записи сообщения в лог
 * \param message сообщение
 */
void Logger::logMessage(const QString &message)
{
    QMutexLocker locker(&mutex);
    QString timeStamped = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    stream << timeStamped << " " << message << "\n";
    stream.flush();
}
