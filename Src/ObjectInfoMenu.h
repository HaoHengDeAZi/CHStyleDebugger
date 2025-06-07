/**
  * 自定义对象信息菜单
  * @author   	ch
  *
*/

#ifndef OBJECTINFOMENU_H
#define OBJECTINFOMENU_H

#include <QMap>
#include <QWidget>
#include <QPointer>
#include <QEventLoop>
#include <QListWidgetItem>

namespace Ui {
class ObjectInfoMenu;
}
class CHPushButton;

class ObjectInfoMenu : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectInfoMenu(QWidget *parent = nullptr);
    ~ObjectInfoMenu();

    void addWidget(QWidget* widget);
    void clear();

    QWidget* exec(const QPoint& p);
    int count();

Q_SIGNALS:
    void selectedWidget(QPointer<QWidget> widget);
    void hoveredWidget(QPointer<QWidget> widget);

protected:
    void hideEvent(QHideEvent *event) override;

private:
    void initWidget();

private:
    Ui::ObjectInfoMenu *ui;

    QEventLoop eventLoop;
    QPointer<QWidget> m_currentWidget;
    QPointer<QWidget> m_selectWidget;
    QMap<CHPushButton*, QPointer<QWidget>> m_itemWidget;
};

#endif // OBJECTINFOMENU_H
