/**
  * 插件头文件
  * @author   	ch
  *
*/

#ifndef CHRuntimeStyleEditor_H
#define CHRuntimeStyleEditor_H

#include <QObject>
#include <QtPlugin>
#include "CHStyleDebuggerInterface.h"

class CHStyleDebugger : public QObject, CHStyleDebuggerInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID CHStyleDebuggerInterface_iid FILE "CHStyleDebugger.json")
    Q_INTERFACES(CHStyleDebuggerInterface)

public:
    CHStyleDebugger(QObject *parent = 0);
    void setCustomStyleSheetFunction(void (*f)(const QString& styleSheet) = Q_NULLPTR);
};

#endif // CHRuntimeStyleEditor_H
