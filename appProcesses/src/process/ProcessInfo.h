#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QString>
#include <QDebug>
#include <QList>
#include <windows.h>

struct ProcessInfo{
    DWORD mPid;
    QString mProcessName;
    QString mPath;
    SIZE_T mMemoryUsage;
    int mThreadCount;
    QString mStartTime;

    QString getTextValue(int index) const{
        switch (index){
        case 0: return QString("%1").arg(mPid);
        case 1: return mProcessName;
        case 2: return mPath;
        case 3: return QString("%1").arg(mMemoryUsage);
        case 4: return QString("%1").arg(mThreadCount);
        case 5: return mStartTime;
        }
        return QString();
    }
    void print(){
        qDebug()<<mPid<<mProcessName<<mPath<<mMemoryUsage<<mThreadCount<<mStartTime;
    }
};

#endif // PROCESSINFO_H
