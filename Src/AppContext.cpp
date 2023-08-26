#include <QFile>
#include <iostream>
#include <QSettings>
#include <QTextStream>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include "AppContext.h"
#include "StyleDebugger.h"

AppContext* AppContext::s_pInstance = Q_NULLPTR;
QMutex AppContext::s_mtxInstance;

QtMessageHandler g_messageHandler = Q_NULLPTR;

AppContext::AppContext(QObject* parent)
    : QObject(parent)
    , m_pStyleDebugger(Q_NULLPTR)
    , m_customStyleSheet(Q_NULLPTR)
{

}

AppContext::~AppContext()
{

}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
        case QtDebugMsg:
        case QtWarningMsg:
        {
            Q_EMIT appContext->receiveDebugInfo(QString(context.file), QString(localMsg.constData()));
            break;
        }
    }

    g_messageHandler(type, context, msg);
}

//初始化
void AppContext::init()
{
    //配置
    settings(true);
    qssConfig(true);

    //调试工具
    if (m_pStyleDebugger == Q_NULLPTR)
    {
        m_pStyleDebugger = new StyleDebugger(Q_NULLPTR);
    }

    //事件循环
    qApp->installEventFilter(this);
    // 检测输出,用于判断是否有qss编写错误
    g_messageHandler = qInstallMessageHandler(myMessageOutput);
}

//获取单例对象
AppContext* AppContext::getInstance()
{
    if (Q_NULLPTR == s_pInstance)
    {
        QMutexLocker locker(&s_mtxInstance);

        if (Q_NULLPTR == s_pInstance)
        {
            s_pInstance = new AppContext();
        }
    }

    return s_pInstance;
}

//释放单例对象
void AppContext::freeInstance()
{
    delete s_pInstance;
    s_pInstance = Q_NULLPTR;
}

//获取自己的Qss
QString AppContext::getSelfQssInfo() const
{
    //获取样式表文件
    QStringList lstFileName;
    lstFileName.append(QString(":/Qss/CHStyleDebuggerCommon.qss"));

    //读取样式表文件
    QFile fleQss;
    QString strStyleSheet;
    int nCount = lstFileName.count();
    for (int i = 0; i < nCount; ++i)
    {
        fleQss.setFileName(lstFileName.at(i));
        fleQss.open(QFile::ReadOnly);
        if (fleQss.isOpen())
        {
            QTextStream in(&fleQss);
            strStyleSheet.append(in.readAll());
            fleQss.close();
        }
    }

    return strStyleSheet;
}

ST_CONFIG AppContext::settings(bool refresh)
{
    if (!refresh)
    {
        return m_softwareConfig;
    }

    //软件信息
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CHStyleDebugger", "SoftwareConfig");
    m_softwareConfig.mainTop = settings.value("Config/mainTop", true).toBool();
    m_softwareConfig.propertyTop = settings.value("Config/propertyTop", true).toBool();
    m_softwareConfig.qssEditorTop = settings.value("Config/qssEditorTop", true).toBool();

    m_softwareConfig.mainOpacity = settings.value("Config/mainOpacity", 1.0).toDouble();
    m_softwareConfig.propertyOpacity = settings.value("Config/propertyOpacity", 1.0).toDouble();
    m_softwareConfig.qssEditorOpacity = settings.value("Config/qssEditorOpacity", 1.0).toDouble();

    m_softwareConfig.mainShortcut = settings.value("Config/mainShortcut", "ALT+1").toString();
    m_softwareConfig.propertyShortcut = settings.value("Config/propertyShortcut", "ALT+2").toString();
    m_softwareConfig.qssEditorShortcut = settings.value("Config/qssEditorShortcut", "ALT+3").toString();

    m_softwareConfig.objectBorderColor = settings.value("Config/objectBorderColor", "#FF3399").toString();
    m_softwareConfig.objectBorderWidth = settings.value("Config/objectBorderWidth", 1).toInt();
    m_softwareConfig.objectKeyShortcut = settings.value("Config/objectKeyShortcut", "CTRL+B").toString();

    m_softwareConfig.showTrayIcon = settings.value("Config/showTrayIcon", true).toBool();
    m_softwareConfig.showMainWindow = settings.value("Config/showMainWindow", true).toBool();

    return m_softwareConfig;
}

void AppContext::setSettings(const ST_CONFIG& config)
{
    //保存在全局的文件夹下
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CHStyleDebugger", "SoftwareConfig");
    settings.setValue("Config/mainTop", config.mainTop);
    settings.setValue("Config/propertyTop", config.propertyTop);
    settings.setValue("Config/qssEditorTop", config.qssEditorTop);
    settings.setValue("Config/mainOpacity", config.mainOpacity);
    settings.setValue("Config/propertyOpacity", config.propertyOpacity);
    settings.setValue("Config/qssEditorOpacity", config.qssEditorOpacity);
    settings.setValue("Config/mainShortcut", config.mainShortcut);
    settings.setValue("Config/propertyShortcut", config.propertyShortcut);
    settings.setValue("Config/qssEditorShortcut", config.qssEditorShortcut);
    settings.setValue("Config/objectBorderColor", config.objectBorderColor.name());
    settings.setValue("Config/objectBorderWidth", config.objectBorderWidth);
    settings.setValue("Config/objectKeyShortcut", config.objectKeyShortcut);
    settings.setValue("Config/showTrayIcon", config.showTrayIcon);
    settings.setValue("Config/showMainWindow", config.showMainWindow);

    m_softwareConfig = config;

    Q_EMIT updateSettings();
}

ST_QSS_CONFIG AppContext::qssConfig(bool refresh)
{
    if (!refresh)
    {
        return m_qssConfig;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qGuiApp->applicationName(), "SoftwareConfig");
    m_qssConfig.qssFiles = settings.value("Config/qssFiles").toStringList();
    m_qssConfig.scaleType = settings.value("Config/scaleType", 0).toInt();
    m_qssConfig.sp = settings.value("Config/sp", 1.0).toDouble();
    m_qssConfig.dp = settings.value("Config/dp", 1.0).toDouble();
    m_qssConfig.autoRefresh = settings.value("Config/autoRefresh", true).toBool();

    return m_qssConfig;
}

void AppContext::setQssConfig(const ST_QSS_CONFIG& config)
{
    //保存在具体的程序下
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qGuiApp->applicationName(), "SoftwareConfig");
    settings.setValue("Config/qssFiles", config.qssFiles);
    settings.setValue("Config/scaleType", config.scaleType);
    settings.setValue("Config/sp", config.sp);
    settings.setValue("Config/dp", config.dp);
    settings.setValue("Config/autoRefresh", config.autoRefresh);

    m_qssConfig = config;

    Q_EMIT updateQssConfig();
}

void AppContext::setCustomStyleSheetFunction(void (*f)(const QString&))
{
    m_customStyleSheet = f;
}

void AppContext::setStyleSheet(const QString& styleSheet)
{
    if (m_customStyleSheet == Q_NULLPTR)
    {
        qApp->setStyleSheet(styleSheet);
    }
    else
    {
        m_customStyleSheet(styleSheet);
    }
}

//事件过滤
bool AppContext::eventFilter(QObject* obj, QEvent *event)
{
    QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(event);
    if (pKeyEvent == Q_NULLPTR || event->type() != QEvent::ShortcutOverride)
    {
        return QObject::eventFilter(obj, event);
    }

    QKeySequence tmp = QKeySequence(pKeyEvent->modifiers() + pKeyEvent->key());
    QString strKey = tmp.toString().toUpper();

    if (m_softwareConfig.mainShortcut.remove("").toUpper() == strKey)
    {
        if (m_pStyleDebugger->isVisible())
        {
            m_pStyleDebugger->hide();
        }
        else
        {
            m_pStyleDebugger->show();
            m_pStyleDebugger->activateWindow();
        }
        return true;
    }
    if (m_softwareConfig.propertyShortcut.remove("").toUpper() == strKey)
    {
        m_pStyleDebugger->togglePropertyPanelVisible();
        return true;
    }

    if (m_softwareConfig.qssEditorShortcut.remove("").toUpper() == strKey)
    {
        m_pStyleDebugger->toggleQssEditorVisible();
        return true;
    }

    if (m_softwareConfig.objectKeyShortcut.remove("").toUpper() == strKey)
    {
       m_pStyleDebugger->selectObject(true);
       return true;
    }

    return QObject::eventFilter(obj, event);
}
