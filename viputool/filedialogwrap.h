#pragma once
#include <QObject>
#include <QUrl>
#include <QSettings>
#include <QFileDialog>

/**
 * @brief 包装QFileDialog在QML中使用
 * 文档指出我们不应该在qml逻辑中执行自己的事件循环，所以通过信号槽异步调用对话框exec
 * 而操作结果也只能通过信号传递，不能像cpp代码那样exec后面直接获取结果
 */
class FileDialogWrap : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int mode READ getMode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QStringList nameFilters READ getNameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(QUrl currentUrl READ getCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(QList<QUrl> currentUrls READ getCurrentUrls NOTIFY currentUrlsChanged)
public:
    enum WorkMode {
        SelectFile, //单选文件
        SelectFiles, //多选文件
        SelectDir, //选择目录
        SaveFile //保存文件
    };
    Q_ENUM(WorkMode)
public:
    explicit FileDialogWrap(QObject *parent = nullptr);
    ~FileDialogWrap();

    //工作模式，枚举WorkMode
    int getMode() const;
    void setMode(int mode);

    //标题设置
    QString getTitle() const;
    void setTitle(const QString &title);

    //后缀过滤
    QStringList getNameFilters() const;
    void setNameFilters(const QStringList &filters);

    //单选结果
    QUrl getCurrentUrl() const;
    void setCurrentUrl(const QUrl &url);

    //多选结果
    QList<QUrl> getCurrentUrls() const;
    void setCurrentUrls(const QList<QUrl> &urls);

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    //打开前选中某个路径，带文件名则作为选中的文件名
    Q_INVOKABLE void selectUrl(const QUrl &url);

    //桌面路径
    Q_INVOKABLE static QString desktopPath();
    Q_INVOKABLE static QUrl desktopUrl();
    //设置ini配置路径，qml初始化之前cpp中设置
    static void initSetting(const QString &filepath, const QSettings::Format &format = QSettings::IniFormat);

private:
    QString getDefaultPath();
    void setDefaultPath(const QString &dirpath);

private slots:
    void openDialog();

signals:
    void __openRequest();
    void __closeRequest();
    void accepted();
    void rejected();

    void modeChanged();
    void titleChanged();
    void nameFiltersChanged();
    void currentUrlChanged();
    void currentUrlsChanged();

private:
    //ini配置文件路径
    inline static QString iniPath;
    inline static QSettings::Format iniFormat{QSettings::IniFormat};
    //对话框
    QFileDialog *dialog;
    //模式，选择文件还是目录等
    int workMode{FileDialogWrap::SelectFile};
    //文件名过滤
    QStringList nameFilters{"All (*.*)"};
    //单选结果，当前选中路径
    QUrl currentUrl;
    //多选结果
    QList<QUrl> currentUrls;
    //selectUrl可设置，下次弹出时生效
    QUrl nextUrl;
};
