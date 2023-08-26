#include "CHPushButton.h"

CHPushButton::CHPushButton(QWidget* parent)
    : QPushButton(parent)
{

}

void CHPushButton::enterEvent(QEvent* event)
{
    Q_EMIT hovered(true);
    QPushButton::enterEvent(event);
}

void CHPushButton::leaveEvent(QEvent* event)
{
    Q_EMIT hovered(false);
    QPushButton::leaveEvent(event);
}
