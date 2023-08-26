#include "mainwindow.h"
#include <QApplication>

#include <QtWidgets>
#include "../Include/CHStyleDebuggerInterface.h"

void test(const QString& styleSheet)
{
    qApp->setStyleSheet(styleSheet);
}

bool loadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("ChPlugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        //如果没有自定义样式转换就不需要以下的函数
        if (plugin)
        {
            CHStyleDebuggerInterface* styleInterface = qobject_cast<CHStyleDebuggerInterface *>(plugin);
            if (styleInterface != Q_NULLPTR)
            {
                styleInterface->setCustomStyleSheetFunction(test);
            }
        }
    }

    return false;
}

// 获取样式文件名
QStringList getStyleFileName()
{
    QStringList lstFileName;
    lstFileName.append(qApp->applicationDirPath() + "/" + "TmpQssFile/Common.qss");
    return lstFileName;
}

// 获取样式表
QString getStyleSheet()
{
    QFile fleQss;
    QString strStyleSheet;
    QStringList lstFileName = getStyleFileName();
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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置样式表
    a.setStyleSheet(getStyleSheet());

    MainWindow w;
    w.show();

    //加载插件
    loadPlugin();

    return a.exec();
}
