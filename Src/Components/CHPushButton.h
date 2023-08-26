/**
  * 基础按钮
  * @author   	ch
  *
*/

#ifndef CHPUSHBUTTON_H
#define CHPUSHBUTTON_H

#include <QObject>
#include <QPushButton>

class CHPushButton : public QPushButton
{
    Q_OBJECT
public:
    CHPushButton(QWidget* parent = Q_NULLPTR);

Q_SIGNALS:
    void hovered(bool hovered);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

};

#endif // CHPUSHBUTTON_H
