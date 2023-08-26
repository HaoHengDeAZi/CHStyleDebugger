/**
  * 全局对象选择器
  * @author   	ch
  *
*/

#ifndef CGLOBALSELECTOR_H
#define CGLOBALSELECTOR_H

#include <QObject>
class CGlobalSelectorPrivate;

class CGlobalSelector : public QObject
{
    Q_OBJECT
public:
    explicit CGlobalSelector(QObject *parent = nullptr);
    virtual ~CGlobalSelector();

    //设置不相应事件列表
    void setNotHandleEventList(const QList<QWidget*> widgets);
    //设置选择状态
    void setSelectState(const bool enabled = true);

Q_SIGNALS:
    //选中
    void selectedWidget(QWidget* object);
    //选择状态改变
    void selectStateChanged(bool enable);

protected:
    bool eventFilter(QObject* obj, QEvent *event);

private:
    CGlobalSelectorPrivate* m_p;
};

#endif // CGLOBALSELECTOR_H
