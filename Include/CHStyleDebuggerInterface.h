/**
  * 美化和调试工具
  * @author   	ch
  * @detail
  *     1. 快速定位Qt对象和对象树关系，辅助代码方便定位对象。
  *     2. 找到对象后可以跟Desginer一样设置对象属性，实时查看结果
  *     3. 内置qss编辑器，支持qss编写于校验
  *     4. 支持自定义样式
  *     5. 自己体验
*/

#ifndef CHRUNTIMESTYLEINTERFACE_H
#define CHUNTIMESTYLEINTERFACE_H

#include <QWidget>

class CHStyleDebuggerInterface
{
public:
    virtual ~CHStyleDebuggerInterface() {}
    virtual void setCustomStyleSheetFunction(void (*f)(const QString& styleSheet) = Q_NULLPTR) = 0;
};

QT_BEGIN_NAMESPACE

#define CHStyleDebuggerInterface_iid "org.ch.azi.Interface"

Q_DECLARE_INTERFACE(CHStyleDebuggerInterface, CHStyleDebuggerInterface_iid)
QT_END_NAMESPACE

#endif // CHRUNTIMESTYLEINTERFACE_H
