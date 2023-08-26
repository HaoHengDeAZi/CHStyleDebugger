/**
  * 属性面板
  * @author   	ch
  *
*/

#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include <QMap>
#include <QVariant>
#include <QWidget>

namespace Ui {
class CPropertyPanel;
}

typedef struct stHistoryValue
{
    QVariant oldValue;
    QVariant newValue;
} ST_HISTORY_VALUE;

typedef struct stProperty
{
    QString strPropertyName;
    ST_HISTORY_VALUE stValues;
} ST_PROPERTY;

class CPropertyPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CPropertyPanel(QWidget *parent = 0);
    ~CPropertyPanel();

    QWidget* objectWidget();

public Q_SLOTS:
    //set Hightlight widget
    void setWidget(QWidget* pW);

Q_SIGNALS:
    void visibleChanged(const bool visible);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    void on_btnAdjustSize_clicked();

private:
    void initWidget();
    void changedInfo(QWidget* pW, const QString& strName, const QVariant value);
    void printLog();

private:   
    QWidget* m_pTarget;
    bool m_hasMove;
    Ui::CPropertyPanel *ui;

    QMap<void*, QList<ST_PROPERTY>> m_mapObjectValues;  //Changed history
    QMap<void*, QLayout*> m_mapObjectLayout;            //Object's Layout
    QList<QWidget*> m_lsPropertyItem;
};

#endif // PROPERTYPANEL_H
