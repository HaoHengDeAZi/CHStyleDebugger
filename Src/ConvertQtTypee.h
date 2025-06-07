/**
  * Qt字符类型转换
  * @author   	ch
  *
*/

#ifndef CONVERTQTTYPEE_H
#define CONVERTQTTYPEE_H

#include <QString>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QMargins>
#include <QColor>

QString rect2String(QRect rect);
QString rectF2String(QRectF rect);
QRect string2Rect(QString str);
QRectF string2RectF(QString str);

QString point2String(QPoint rect);
QString pointF2String(QPointF rect);
QPoint string2Point(QString str);
QPointF string2PointF(QString str);

QString size2String(QSize rect);
QSize string2Size(QString str);

QString Margins2String(QMargins rect);
QMargins string2Margins(QString str);

QString color2String(QColor color);
QColor string2Color(QString str);
#endif // CONVERTQTTYPEE_H
