#include "ProcessInformationHandler.h"

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::ProcessInformationHandler Конструктор
 * \param parent
 */
ProcessInformationHandler::ProcessInformationHandler(QObject *parent)
    : QObject{parent}
{

}

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::~ProcessInformationHandler Деструктор
 */
ProcessInformationHandler::~ProcessInformationHandler()
{

}

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::processing Слот обработки параметров системы
 */
void ProcessInformationHandler::processing()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        emit sendLogMessage("Не удалося получить снимок процессов!");
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    QList<ProcessInfo>listProcInfo;
    if (Process32First(hSnapshot, &pe)) {
        do {
            ProcessInfo procInfo;
            procInfo.mPid = pe.th32ProcessID;
            procInfo.mProcessName = QString::fromWCharArray(pe.szExeFile);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procInfo.mPid);
            if (hProcess) {
                procInfo.mPath = getProcessPath(procInfo.mPid);
                procInfo.mMemoryUsage  = int(getMemoryUsage(hProcess)/1024);
                procInfo.mThreadCount = pe.cntThreads;
                procInfo.mStartTime = getStartTime(hProcess);
                CloseHandle(hProcess);
            }
        listProcInfo.append(procInfo);
        } while (Process32Next(hSnapshot, &pe));
    }
    emit sendProcessInfo(listProcInfo);
    CloseHandle(hSnapshot);
}

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::getProcessPath Метод возвращает путь к процессу
 * \param pid
 * \return
 */
QString ProcessInformationHandler::getProcessPath(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        TCHAR path[MAX_PATH];
        if (GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH)) {
            CloseHandle(hProcess);
            return QString::fromWCharArray(path);
        }
        CloseHandle(hProcess);
    }
    return "N/A";
}

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::getMemoryUsage Метод определяет  размер занятой оперативной памяти процессом
 * \param hProcess
 * \return
 */
SIZE_T ProcessInformationHandler::getMemoryUsage(HANDLE hProcess)
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

//*******************************************************************************
/*!
 * \brief ProcessInformationHandler::getStartTime Метод определения момента времени
 * \param hProcess
 * \return
 */
QString ProcessInformationHandler::getStartTime(HANDLE hProcess)
{
    FILETIME ftCreate, ftExit, ftKernel, ftUser;
    if (GetProcessTimes(hProcess, &ftCreate, &ftExit, &ftKernel, &ftUser)) {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftCreate, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
        QDateTime dt = QDateTime(QDate(stLocal.wYear, stLocal.wMonth, stLocal.wDay),
                                 QTime(stLocal.wHour, stLocal.wMinute, stLocal.wSecond));
        return dt.toString("yyyy-MM-dd HH:mm:ss");
    }
    return "N/A";
}
