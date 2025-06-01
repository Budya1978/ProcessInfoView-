#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT
public:
    /*! @brief Конструктор класса ведущий лог сообщений */
    explicit Logger(QObject *parent = nullptr);
    /*! @brief Деструктор */
    ~Logger();

public slots:
    /*! @brief Слот записи сообщения в лог */
    void logMessage(const QString &message);

private:
    //!Лог файл
    QFile logFile;
    //!Поток для записи
    QTextStream stream;
    //!Мютекс для синхронизации доступа к файлу
    QMutex mutex;
};

#endif // LOGGER_H
