#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractTableModel>
#include "ProcessInfo.h"

class ProcessModel : public QAbstractTableModel {
    Q_OBJECT

public:
    /*! @brief Конструктор класса создающий модель для отображения информации о процессах */
    explicit ProcessModel(QObject *parent = nullptr);
    /*! @brief Деструктор */
    ~ProcessModel();
    /*! @brief Метод возвращает количество строк */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /*! @brief Метод возвращает количество столбцов */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /*! @brief Метод возвращает данные о процессах */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*! @brief Метод возвращает данные заголовка модели */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    /*! @brief Метод выполняет обновление данных модели при каждом считывании из системы */
    void updateProcessList(const QList<ProcessInfo> &newList);

private:
    //!Список описаний процессов
    QList<ProcessInfo> m_processes;
    //!Список имен столбцов заголовка таблицы
    static const QStringList columnHeaders;
};


#endif // PROCESSMODEL_H
