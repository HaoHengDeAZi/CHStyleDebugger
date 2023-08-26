#include "AppContext.h"
#include "CHStyleDebugger.h"

CHStyleDebugger::CHStyleDebugger(QObject *parent) :
    CHStyleDebuggerInterface()
{
    Q_UNUSED(parent);
    //实例化
    appContext->init();
}

void CHStyleDebugger::setCustomStyleSheetFunction(void (*f)(const QString&))
{
    appContext->setCustomStyleSheetFunction(f);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(CHStyleDebugger, CHStyleDebugger)
#endif // QT_VERSION < 0x050000
