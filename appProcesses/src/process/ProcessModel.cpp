#include "ProcessModel.h"

const QStringList ProcessModel::columnHeaders = {
    "PID", "Имя процесcа", "Путь", "Используемая\nпамять", "Колличество\nпотоков", "Время\nстарта"
};

//*********************************************************************
/*!
 * \brief ProcessModel::ProcessModel Конструктор класса создающий модель для отображения информации о процессах
 * \param parent родитель
 */
ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent) {

}

//*********************************************************************
/*!
 * \brief ProcessModel::~ProcessModel Деструктор
 */
ProcessModel::~ProcessModel(){

}

//*********************************************************************
/*!
 * \brief ProcessModel::rowCount Метод возвращает количество строк
 * \return Количество строк
 */
int ProcessModel::rowCount(const QModelIndex &/*parent*/) const {
    return m_processes.size();
}

//*********************************************************************
/*!
 * \brief ProcessModel::columnCount Метод возвращает количество столбцов
 * \return Количество столбцов
 */
int ProcessModel::columnCount(const QModelIndex &/*parent*/) const {
    return columnHeaders.size();
}

//*********************************************************************
/*!
 * \brief ProcessModel::data Метод возвращает данные о процессах
 * \param index  индекс ячейки
 * \param role   роль данных
 * \return
 */
QVariant ProcessModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_processes.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return m_processes[index.row()].getTextValue(index.column());
    }

    return QVariant();
}

//*********************************************************************
/*!
 * \brief ProcessModel::headerData Метод возвращает данные заголовка модели
 * \param section       номер столбца в этом случае
 * \param orientation   ориентация
 * \param role          роль
 * \return
 */
QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && section < columnHeaders.size())
            return columnHeaders[section];
        if (orientation == Qt::Vertical)
            return section + 1;
    }
    return QVariant();
}

//*********************************************************************
/*!
 * \brief ProcessModel::updateProcessList   Метод выполняет обновление данных модели при каждом считывании из системы
 * \param newList   список описаний процессов
 */
void ProcessModel::updateProcessList(const QList<ProcessInfo> &newList) {
    QHash<DWORD, int> oldIndexMap;
    for (int i = 0; i < m_processes.size(); ++i) {
        oldIndexMap[m_processes[i].mPid] = i;
    }
    QList<ProcessInfo> updatedProcesses = m_processes;
    QSet<int> changedRows;
    QSet<int> newRows;
    for (const auto &proc : newList) {
        if (oldIndexMap.contains(proc.mPid)) {
            int idx = oldIndexMap[proc.mPid];
            if (proc.mMemoryUsage != m_processes[idx].mMemoryUsage ||
                proc.mThreadCount != m_processes[idx].mThreadCount ||
                proc.mPath != m_processes[idx].mPath ||
                proc.mProcessName != m_processes[idx].mProcessName ||
                proc.mStartTime != m_processes[idx].mStartTime)
            {
                updatedProcesses[idx] = proc;
                changedRows.insert(idx);
            }
            oldIndexMap.remove(proc.mPid); // Остались только удалённые
        } else {
            updatedProcesses.append(proc);
            newRows.insert(updatedProcesses.size() - 1);
        }
    }
    QList<int> indicesToRemove = oldIndexMap.values();
    std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<>());
    for (int idx : indicesToRemove) {
        beginRemoveRows(QModelIndex(), idx, idx);
        updatedProcesses.removeAt(idx);
        endRemoveRows();
    }
    for (int row : changedRows) {
        m_processes[row] = updatedProcesses[row];
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
    for (int row : newRows) {
        if(row < newList.count()){
        beginInsertRows(QModelIndex(), row, row);
        m_processes.append(newList[row]);
        endInsertRows();
        }
    }
    m_processes = updatedProcesses;
}
