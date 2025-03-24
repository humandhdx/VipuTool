#include "utils/qfilesystemmonitor.hpp"
#include <QDebug>
#include <QDir>
#include <unistd.h>
#include <iostream>
#include <libgen.h>
#include <utils/ReadAndWriteFile.hpp>

#define DELAY_MS_JUDGE_FILE_CHANGE 10

QFileSystemMonitor *QFileSystemMonitor::instance()
{
    static QFileSystemMonitor instance{nullptr};
    return &instance;
}

#include <QThread>
QFileSystemMonitor::~QFileSystemMonitor()
{
    qWarning() << __FUNCTION__ << " - 0!";
    QThread::sleep(2);
}

bool QFileSystemMonitor::Register_Callback_On_File_Modified(const std::string &filePath_relative, CALLBACK_ON_FILE_CHANGED cb)
{
    if(0 == filePath_relative.size())
    {
        qDebug() << __FUNCTION__ << " - given file relative path should not be empty!";
        return false;
    }
    if(('/' == filePath_relative.at(0)))
    {
        qDebug() << __FUNCTION__ << " - given file relative path" << QString::fromStdString(filePath_relative) << " should not start with '/'!";
        return false;
    }

    QString fileAbsPath = QString::fromStdString(this->path_binary_dir_ + filePath_relative);
    // qDebug() << "fileAbsPath:" << fileAbsPath;
    QFileInfo file_info{fileAbsPath};
    QString dirPath = file_info.dir().absolutePath();
    QFileInfo dir_info{dirPath};
    if(!dir_info.exists())
    {
        qDebug() << __FUNCTION__ << " - the give file dir" << dirPath << "not exist!";
        return false;
    }
    else if (map_WatchFilePath_To_Info_.contains(fileAbsPath))
    {
        qDebug() << __FUNCTION__ << " - the give file '" << fileAbsPath << "' already registered!";
        return false;
    }

    QDateTime lastModifyTime = file_info.exists()?
        file_info.lastModified() : TIMESTAMP_MISSING_FILE;

    if(!map_dirPath_To_WatchFilePathSet_.contains(dirPath))
    {
        map_dirPath_To_WatchFilePathSet_.insert(dirPath, QSet<QString>());
    }

    (*(map_dirPath_To_WatchFilePathSet_.find(dirPath))).insert(fileAbsPath);
    map_WatchFilePath_To_Info_.insert(fileAbsPath, {lastModifyTime, cb});

    QSharedPointer<QTimer> timer = QSharedPointer<QTimer>::create(this);
    timer->setSingleShot(true);
    connect(timer.get(), &QTimer::timeout, this, std::bind(&QFileSystemMonitor::SlotFunction_DirChanged, this, dirPath));
    map_dirPath_To_Qtimer_.insert(dirPath, timer);
    // qDebug() << fileAbsPath << " modified time" << lastModifyTime;
    qFileWatcher_.addPath(dirPath);
    cb(fileAbsPath.toStdString());

    return true;
}

void QFileSystemMonitor::Deregister_Callback_On_File_Modified(const std::string &filePath_relative)
{
    if(0 == filePath_relative.size())
    {
        qDebug() << __FUNCTION__ << " - given file relative path should not be empty!";
        return ;
    }
    if(('/' == filePath_relative.at(0)))
    {
        qDebug() << __FUNCTION__ << " - given file relative path" << QString::fromStdString(filePath_relative) << " should not start with '/'!";
        return ;
    }

    QString fileAbsPath = QString::fromStdString(this->path_binary_dir_ + filePath_relative);
    // qDebug() << "fileAbsPath:" << fileAbsPath;
    QFileInfo file_info{fileAbsPath};
    QString dirPath = file_info.dir().absolutePath();
    QFileInfo dir_info{dirPath};
    if(!map_dirPath_To_WatchFilePathSet_.contains(dirPath))
    {
        return;
    }
    auto watchFilePathSet_byDir = (*(map_dirPath_To_WatchFilePathSet_.find(dirPath)));

    watchFilePathSet_byDir.remove(fileAbsPath);
    map_WatchFilePath_To_Info_.remove(fileAbsPath);
    if(0 == watchFilePathSet_byDir.size())
    {
        map_dirPath_To_WatchFilePathSet_.remove(dirPath);
        (*map_dirPath_To_Qtimer_.find(dirPath))->disconnect();
        qFileWatcher_.removePath(dirPath);
        map_dirPath_To_Qtimer_.remove(dirPath);
    }
}

QFileSystemMonitor::QFileSystemMonitor(QObject *parent)
    : QObject{parent}
{
    QFileInfo bin_file_info{QString::fromStdString(Utilities::BIN_ABSOLUTE_PATH)};
    QString q1 = bin_file_info.absoluteFilePath();
    QString q2 = bin_file_info.dir().absolutePath();
    std::string p1 = bin_file_info.dir().absolutePath().toStdString();
    std::string p2 = p1.append("/");
    this->path_binary_dir_ =  bin_file_info.dir().absolutePath().toStdString().append("/");
    //use timer delay handle directory change,
    //since single operation in files system will lead to consecutive SIGNAL of 'directoryChanged'
    connect(&qFileWatcher_, &QFileSystemWatcher::directoryChanged, this, [&](const QString &dir_path){
        auto itr = map_dirPath_To_Qtimer_.find(dir_path);
        if(itr != map_dirPath_To_Qtimer_.end())
        {
            (*(itr))->start(DELAY_MS_JUDGE_FILE_CHANGE);
        }
    });
}

void QFileSystemMonitor::SlotFunction_DirChanged(const QString &dir_path)
{
    qDebug() << __FUNCTION__ << " - " <<dir_path;

    QSet<QString> &set_WatchedFilePath = (*(map_dirPath_To_WatchFilePathSet_.find(dir_path)));

    for(const QString& file_Path: set_WatchedFilePath)
    {
        QFileInfo qfileInfo_lastest{file_Path};
        FileWatchInfo& ref_file_Info = *map_WatchFilePath_To_Info_.find(file_Path);

        // if(!qfileInfo_lastest.exists())
        // {
        //     ref_file_Info.last_modified_time = TIMESTAMP_MISSING_FILE;
        //     // continue;
        // }

        QDateTime recorded_last_modify_time = ref_file_Info.last_modified_time;
        QDateTime current_last_modify_time = qfileInfo_lastest.exists()?
                                                 qfileInfo_lastest.lastModified():TIMESTAMP_MISSING_FILE;
        //modified
        auto ms_elapsed = recorded_last_modify_time.msecsTo(current_last_modify_time);
        // qDebug() << file_Path << " modified time" << current_last_modify_time;
        // qDebug() << file_Path << " modified " << ms_elapsed << "ms ago";
        if(DELAY_MS_JUDGE_FILE_CHANGE < ms_elapsed)
        {
            // qDebug() << file_Path << "added or modified";
            ref_file_Info.last_modified_time = current_last_modify_time;
            ref_file_Info.callback_onModified(file_Path.toStdString());
        }
        else if(0 > ms_elapsed)
        {
            // qDebug() << file_Path << "deleted";
            ref_file_Info.last_modified_time = current_last_modify_time;
            ref_file_Info.callback_onModified(file_Path.toStdString());
        }
    }
}
