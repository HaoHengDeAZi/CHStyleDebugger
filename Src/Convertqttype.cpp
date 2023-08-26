#include "ConvertQtTypee.h"
#include <QStringList>

QString rect2String(QRect rect)
{
    QString str("%1,%2,%3,%4");
    return str.arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
}

QString rectF2String(QRectF rect)
{
    QString str("%1,%2,%3,%4");
    return str.arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
}

QRect string2Rect(QString str)
{
    QRect rect;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 4)
    {
        return rect;
    }
    rect = QRect(ls.at(0).toInt(), ls.at(1).toInt(), ls.at(2).toInt(), ls.at(3).toInt());
    return rect;
}

QRectF string2RectF(QString str)
{
    QRectF rect;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 4)
    {
        return rect;
    }
    rect = QRectF(ls.at(0).toFloat(), ls.at(1).toFloat(), ls.at(2).toFloat(), ls.at(3).toFloat());
    return rect;
}

QString point2String(QPoint value)
{
    QString str("%1,%2");
    return str.arg(value.x()).arg(value.y());
}

QString pointF2String(QPointF value)
{
    QString str("%1,%2");
    return str.arg(value.x()).arg(value.y());
}

QPoint string2Point(QString str)
{
    QPoint value;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 2)
    {
        return value;
    }
    value = QPoint(ls.at(0).toInt(), ls.at(1).toInt());
    return value;
}

QPointF string2PointF(QString str)
{
    QPointF value;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 2)
    {
        return value;
    }
    value = QPointF(ls.at(0).toFloat(), ls.at(1).toFloat());
    return value;
}

QString size2String(QSize value)
{
    QString str("%1,%2");
    return str.arg(value.width()).arg(value.height());
}

QSize string2Size(QString str)
{
    QSize value;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 2)
    {
        return value;
    }
    value = QSize(ls.at(0).toInt(), ls.at(1).toInt());
    return value;
}

QString Margins2String(QMargins value)
{
    QString str("%1,%2,%3,%4");
    return str.arg(value.left()).arg(value.top()).arg(value.right()).arg(value.bottom());
}

QMargins string2Margins(QString str)
{
    QMargins value;
    QStringList ls = str.remove("(").remove(")").remove(" ").split(",");
    if (ls.length() != 4)
    {
        return value;
    }
    value = QMargins(ls.at(0).toInt(), ls.at(1).toInt(), ls.at(2).toInt(), ls.at(3).toInt());
    return value;
}
