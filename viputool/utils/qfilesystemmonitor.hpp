#ifndef QFILESYSTEMMONITOR_HPP
#define QFILESYSTEMMONITOR_HPP

#include <QObject>
#include <functional>
#include <string>
#include <QMap>
#include <QSet>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QTime>
#include <QSharedPointer>

class QFileSystemMonitor : public QObject
{
    Q_OBJECT

    typedef std::function<void(const std::string &)> CALLBACK_ON_FILE_CHANGED;

public:
    static QFileSystemMonitor *instance();

    ~QFileSystemMonitor();

    bool Register_Callback_On_File_Modified(const std::string &filePath, CALLBACK_ON_FILE_CHANGED cb);

signals:

private:
    explicit QFileSystemMonitor(QObject *parent = nullptr);

    void SlotFunction_FileChanged(const QString &path);
    void SlotFunction_DirChanged(const QString &path);
    void SlotFunction_FileChanged_PostRegistered(const QString &path);

    QFileSystemWatcher qFileWatcher_;

    std::string path_binary_dir_;

    struct FileWatchInfo{
        QDateTime                   last_modified_time;
        CALLBACK_ON_FILE_CHANGED    callback_onModified;
    };

    QMap<QString, FileWatchInfo>            map_WatchFilePath_To_Info_;

    QMap<QString, QSet<QString>>            map_dirPath_To_WatchFilePathSet_;

    QMap<QString, QSharedPointer<QTimer>>   map_dirPath_To_Qtimer_;

    const QDateTime TIMESTAMP_MISSING_FILE = QDateTime::fromSecsSinceEpoch(0);
};

#endif // QFILESYSTEMMONITOR_HPP
