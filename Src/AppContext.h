/**
  * 单例上下文类
  * @author   	ch
  *
*/

#pragma once

#include <QColor>
#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QSystemTrayIcon>

#define appContext AppContext::getInstance()

class StyleDebugger;

typedef struct st_config
{
    bool mainTop;
    bool propertyTop;
    bool qssEditorTop;

    double mainOpacity;
    double propertyOpacity;
    double qssEditorOpacity;

    QString mainShortcut;
    QString propertyShortcut;
    QString qssEditorShortcut;

    QColor objectBorderColor;
    int objectBorderWidth;
    bool objectBorderVisible;
    QString objectKeyShortcut;

    bool showTrayIcon;
    bool showMainWindow;

    bool showMainFps;
    bool showSelectFps;

    st_config()
    {
        mainTop = true;
        propertyTop = true;
        qssEditorTop = true;
        mainOpacity = 1.0;
        propertyOpacity = 1.0;
        qssEditorOpacity = 1.0;
        mainShortcut = "ALT+1";
        propertyShortcut = "ALT+2";
        qssEditorShortcut = "ALT+3";
        objectBorderColor = QColor("#FF3399");
        objectBorderWidth = 1;
        objectBorderVisible = true;
        objectKeyShortcut = "CTRL+B";
        showTrayIcon = true;
        showMainWindow = true;
        showMainFps = true;
        showSelectFps = true;
    }
} ST_CONFIG;

typedef struct st_qssConfig
{
    QStringList qssFiles;

    int scaleType;
    double sp;
    double dp;

    bool autoRefresh;

    st_qssConfig()
    {
        scaleType = 0;
        sp = 1.0;
        dp = 1.0;
        autoRefresh = true;
    }

} ST_QSS_CONFIG;

class AppContext : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AppContext)

public:
    //获取单例对象
    static AppContext* getInstance();
    //释放单例对象
    static void freeInstance();

    //初始化
    void init();

    //获取自己的Qss
    QString getSelfQssInfo() const;

    //系统配置
    ST_CONFIG settings(bool refresh = false);
    void setSettings(const ST_CONFIG& config);

    //Qss配置
    ST_QSS_CONFIG qssConfig(bool refresh = false);
    void setQssConfig(const ST_QSS_CONFIG& config);

    //自定义式样函数
    void setCustomStyleSheetFunction(void (*f)(const QString& styleSheet));
    void setStyleSheet(const QString& styleSheet);

    // 设置fps
    void setFpsValue(int mainFps, int selectFps);

Q_SIGNALS:
    //更新Qss内容
    void updateQssInfo();
    //更新系统设置
    void updateSettings();
    //更新qss设置
    void updateQssConfig();
    //收到调试输出信息
    void receiveDebugInfo(const QString& file, const QString& context);

protected:
    //事件过滤
    bool eventFilter(QObject* obj, QEvent *event);

private:
    AppContext(QObject* parent = Q_NULLPTR);
    ~AppContext();

private:
    static AppContext* s_pInstance;                 //单例对象
    static QMutex s_mtxInstance;                    //单例对象互斥量

    StyleDebugger* m_pStyleDebugger;

    ST_CONFIG m_softwareConfig;                     //配置
    ST_QSS_CONFIG m_qssConfig;                      //QSS配置

    void (*m_customStyleSheet)(const QString& styleSheet);
};
