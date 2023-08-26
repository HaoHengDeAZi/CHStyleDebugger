#include "PropertyPanel.h"
#include "ui_PropertyPanel.h"
#include <QMetaProperty>
#include <QScreen>
#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include "Resources.h"
#include "AppContext.h"
#include "ConvertQtTypee.h"
#include <QDebug>
#include <QDesktopWidget>

CPropertyPanel::CPropertyPanel(QWidget *parent) :
    QWidget(parent),
    m_pTarget(Q_NULLPTR),
    m_hasMove(false),
    ui(new Ui::CPropertyPanel)
{
    ui->setupUi(this);
    initWidget();
}

CPropertyPanel::~CPropertyPanel()
{
    delete ui;

    qDeleteAll(m_lsPropertyItem);
    m_lsPropertyItem.clear();
}

QWidget* CPropertyPanel::objectWidget()
{
    return m_pTarget;
}

void CPropertyPanel::setWidget(QWidget *pW)
{
    m_pTarget = pW;
    qDeleteAll(m_lsPropertyItem);
    m_lsPropertyItem.clear();

    if (pW == Q_NULLPTR)
    {
        return;
    }

    hide();
    //销毁的时候清空
    connect(pW, &QWidget::destroyed, this, [this]()
    {
        m_pTarget = Q_NULLPTR;
        qDeleteAll(m_lsPropertyItem);
        m_lsPropertyItem.clear();
        m_mapObjectValues.clear();
    });

    QObject* target = pW;
    //窗口名称
    QString strClassName = QString::fromLatin1(pW->metaObject()->className());
    setWindowTitle(QString(tr("ClassName：%1，ObjectName：%2").arg(pW->objectName()).arg(strClassName)));

    QStringList lsColor;
    lsColor << "#FFFFDE" << "#FFDEDE" << "#FFE6BF";
    int nCount = target->metaObject()->propertyCount();
    if (nCount > 0)
    {
        QLabel* tip = new QLabel(this);
        tip->setObjectName("lblPropertyTip");
        tip->setText(QString("属性信息"));
        ui->vlayPropery->addWidget(tip);
        m_lsPropertyItem.append(tip);
    }

    //属性信息
    for (int i = 0; i < nCount; ++i)
    {
        QMetaProperty mp = target->metaObject()->property(i);
        if (!mp.isReadable() || !mp.isWritable() || !mp.isValid())
        {
            continue;
        }

        QWidget* pProInfo = new QWidget(this);
        pProInfo->setFixedHeight(36);
        pProInfo->setObjectName(QString("wMyPanelItem%1").arg(0));
        QString strSheet = QString("QWidget#%1, QLabel, QCheckBox{background-color: %2;} QWidget#%1{border-bottom:1px solid #ECECDC;}").arg(pProInfo->objectName()).arg(lsColor.at(0));
        pProInfo->setStyleSheet(strSheet);
        m_lsPropertyItem.append(pProInfo);
        QHBoxLayout* ly = new QHBoxLayout(pProInfo);
        ly->setContentsMargins(6, 2, 3, 2);
        pProInfo->setLayout(ly);
        QLabel* pLblName = new QLabel(pProInfo);
        pLblName->setFixedWidth(145);
        pLblName->setText(mp.name());
        ly->addWidget(pLblName);
        ui->vlayPropery->addWidget(pProInfo);

        QVariant type = mp.type();
        switch (type.type()) {
        case QVariant::Bool:
        {

            QCheckBox* p = new QCheckBox(pProInfo);
            p->setChecked(mp.read(pW).toBool());
            ly->addWidget(p);
            connect(p, &QCheckBox::clicked, pW, [pW, p, pLblName, this]()
            {
                changedInfo(pW, pLblName->text(), p->isChecked());
            }, Qt::UniqueConnection);
            break;
        }
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        {
            if (mp.isEnumType())
            {
                QMetaEnum me = mp.enumerator();
                QStringList lsEnum;
                int nCount = me.keyCount();
                for (int i = 0; i < nCount; i++)
                {
                    lsEnum.append(me.key(i));
                }
                QComboBox* p = new QComboBox(pProInfo);
                p->addItems(lsEnum);
                p->setCurrentText(me.valueToKey(mp.read(pW).toInt()));
                ly->addWidget(p);
                connect(p, &QComboBox::currentTextChanged, pW, [pW, p, pLblName, this, me]()
                {
                    int nValue = me.keyToValue(p->currentText().toLocal8Bit().data());
                    changedInfo(pW, pLblName->text(), nValue);
                }, Qt::UniqueConnection);
                break;
            }

            QLineEdit* p = new QLineEdit(pProInfo);
            QIntValidator* pIntVld = new QIntValidator(pProInfo);
            p->setValidator(pIntVld);
            p->setText(mp.read(pW).toString());
            ly->addWidget(p);
            connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
            {
                changedInfo(pW, pLblName->text(), p->text().trimmed());
            }, Qt::UniqueConnection);
            break;
        }

        case QVariant::String:
        {
            QLineEdit* p = new QLineEdit(pProInfo);
            p->setText(mp.read(pW).toString());
            ly->addWidget(p);
            connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
            {
                changedInfo(pW, pLblName->text(), p->text().trimmed());
            }, Qt::UniqueConnection);
            break;
        }
        case QVariant::Rect:
        case QVariant::RectF:
        {
            QLineEdit* p = new QLineEdit(pProInfo);
            if (type.type() == QVariant::Rect)
            {
                p->setText(rect2String(mp.read(pW).toRect()));
                connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
                {
                    changedInfo(pW, pLblName->text(), string2Rect(p->text().trimmed()));
                }, Qt::UniqueConnection);
            }
            else
            {
                p->setText(rectF2String(mp.read(pW).toRectF()));
                connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
                {
                    changedInfo(pW, pLblName->text(), string2RectF(p->text().trimmed()));
                }, Qt::UniqueConnection);
            }
            ly->addWidget(p);
            break;
        }
        case QVariant::Point:
        case QVariant::PointF:
        {
            QLineEdit* p = new QLineEdit(pProInfo);
            if (type.type() == QVariant::Point)
            {
                p->setText(point2String(mp.read(pW).toPoint()));
                connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
                {
                    changedInfo(pW, pLblName->text(), string2Point(p->text().trimmed()));
                }, Qt::UniqueConnection);
            }
            else
            {
                p->setText(pointF2String(mp.read(pW).toPointF()));
                connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
                {
                    changedInfo(pW, pLblName->text(), string2PointF(p->text().trimmed()));
                }, Qt::UniqueConnection);
            }
            ly->addWidget(p);
            break;
        }
        case QVariant::Size:
        case QVariant::SizeF:
        {
            QLineEdit* p = new QLineEdit(pProInfo);
            p->setText(size2String(mp.read(pW).toSize()));
            connect(p, &QLineEdit::editingFinished, pW, [pW, p, pLblName, this]()
            {
                changedInfo(pW, pLblName->text(), string2Size(p->text().trimmed()));
            }, Qt::UniqueConnection);
            ly->addWidget(p);
            break;
        }

        case QVariant::SizePolicy:
        {
            break;
        }

        default:
        {
            //qDebug() << "Not Support:" << type.type();
            QLabel* pInfo = new QLabel(pProInfo);
            pInfo->setText("暂不支持，后续完善");
            ly->addWidget(pInfo);
            break;
            }
        }

        ly->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Preferred));
    }

    //布局信息
    if (target->inherits("QWidget"))
    {
        QLayout* layout = pW->layout();
        if (layout != Q_NULLPTR)
        {
            QLabel* tip = new QLabel(this);
            tip->setObjectName("lblPropertyTip");
            tip->setText(QString("布局信息"));
            ui->vlayPropery->addWidget(tip);
            m_lsPropertyItem.append(tip);
            // ObjectName
            {
                QWidget* pLayoutInfo = new QWidget(this);
                pLayoutInfo->setFixedHeight(36);
                pLayoutInfo->setObjectName(QString("wMyPanelItem%1").arg(1));
                QString strSheet = QString("QWidget#%1, QLabel, QCheckBox{background-color: %2;} QWidget#%1{border-bottom:1px solid #ECECDC;}").arg(pLayoutInfo->objectName()).arg(lsColor.at(1));
                pLayoutInfo->setStyleSheet(strSheet);
                m_lsPropertyItem.append(pLayoutInfo);
                QHBoxLayout* ly = new QHBoxLayout(pLayoutInfo);
                ly->setContentsMargins(6, 2, 3, 2);
                pLayoutInfo->setLayout(ly);
                QLabel* pLblName = new QLabel(pLayoutInfo);
                pLblName->setText("objectName");
                pLblName->setFixedWidth(145);

                ly->addWidget(pLblName);
                QLineEdit* p = new QLineEdit(pLayoutInfo);
                p->setText(layout->objectName());
                connect(p, &QLineEdit::editingFinished, layout, [layout, pW, p, this]()
                {
                    m_mapObjectLayout.insert(pW, layout);
                    if (!m_mapObjectValues.contains(pW))
                    {
                        m_mapObjectValues.insert(pW, QList<ST_PROPERTY>());
                    }
                    layout->setObjectName(p->text().trimmed());
                    printLog();
                }, Qt::UniqueConnection);
                ly->addWidget(p);
                ui->vlayPropery->addWidget(pLayoutInfo);
            }
            // contextsMargins
            {
                QWidget* pLayoutInfo = new QWidget(this);
                pLayoutInfo->setFixedHeight(36);
                pLayoutInfo->setObjectName(QString("wMyPanelItem%1").arg(1));
                QString strSheet = QString("QWidget#%1, QLabel, QCheckBox{background-color: %2;} QWidget#%1{border-bottom:1px solid #ECECDC;}").arg(pLayoutInfo->objectName()).arg(lsColor.at(1));
                pLayoutInfo->setStyleSheet(strSheet);
                m_lsPropertyItem.append(pLayoutInfo);
                QHBoxLayout* ly = new QHBoxLayout(pLayoutInfo);
                ly->setContentsMargins(6, 2, 3, 2);
                pLayoutInfo->setLayout(ly);
                QLabel* pLblName = new QLabel(pLayoutInfo);
                pLblName->setText("contextsMargins\nleft,top,right,bottom");
                pLblName->setFixedWidth(145);
                ly->addWidget(pLblName);
                QLineEdit* p = new QLineEdit(pLayoutInfo);
                p->setText(Margins2String(layout->contentsMargins()));
                connect(p, &QLineEdit::editingFinished, layout, [layout, pW, p, this]()
                {
                    m_mapObjectLayout.insert(pW, layout);
                    if (!m_mapObjectValues.contains(pW))
                    {
                        m_mapObjectValues.insert(pW, QList<ST_PROPERTY>());
                    }
                    layout->setContentsMargins(string2Margins(p->text().trimmed()));
                    printLog();
                }, Qt::UniqueConnection);
                ly->addWidget(p);
                ui->vlayPropery->addWidget(pLayoutInfo);
            }

            // spacing
            {
                QWidget* pLayoutInfo = new QWidget(this);
                pLayoutInfo->setFixedHeight(36);
                pLayoutInfo->setObjectName(QString("wMyPanelItem%1").arg(1));
                QString strSheet = QString("QWidget#%1, QLabel, QCheckBox{background-color: %2;} QWidget#%1{border-bottom: 1px solid #ECECDC;}").arg(pLayoutInfo->objectName()).arg(lsColor.at(1));
                pLayoutInfo->setStyleSheet(strSheet);
                m_lsPropertyItem.append(pLayoutInfo);
                QHBoxLayout* ly = new QHBoxLayout(pLayoutInfo);
                ly->setContentsMargins(6, 2, 3, 2);
                pLayoutInfo->setLayout(ly);
                QLabel* pLblName = new QLabel(pLayoutInfo);
                pLblName->setText("spacing");
                pLblName->setFixedWidth(145);
                ly->addWidget(pLblName);
                QLineEdit* p = new QLineEdit(pLayoutInfo);
                p->setText(QString::number(layout->spacing()));
                QIntValidator* pIntVld = new QIntValidator(this);
                p->setValidator(pIntVld);
                connect(p, &QLineEdit::editingFinished, layout, [layout, pW, p, this]()
                {
                    m_mapObjectLayout.insert(pW, layout);
                    if (!m_mapObjectValues.contains(pW))
                    {
                        m_mapObjectValues.insert(pW, QList<ST_PROPERTY>());
                    }
                    layout->setSpacing(p->text().trimmed().toInt());
                    printLog();
                }, Qt::UniqueConnection);
                ly->addWidget(p);
                ui->vlayPropery->addWidget(pLayoutInfo);
            }
            //LayoutItem
            int nItemCount = 0;
            for (int i = 0; i < layout->count(); i++)
            {
                QLayoutItem* pLayoutItem = layout->itemAt(i);
                QSpacerItem* pSpacerItem = pLayoutItem->spacerItem();
                if (!pSpacerItem)
                {
                    continue;
                }
                nItemCount++;
                {
                    QLabel* tip = new QLabel(this);
                    tip->setObjectName("lblPropertyTip");
                    tip->setText(QString("弹簧%1(注意:弹簧不生成参考代码!)").arg(nItemCount));
                    ui->vlayPropery->addWidget(tip);
                    m_lsPropertyItem.append(tip);

                    QWidget* pLayoutInfo = new QWidget(this);
                    pLayoutInfo->setFixedHeight(36);
                    pLayoutInfo->setObjectName(QString("wMyPanelItem%1").arg(1));
                    QString strSheet = QString("QWidget#%1, QLabel, QCheckBox{background-color: %2;} QWidget#%1{border-bottom:1px solid #ECECDC;}").arg(pLayoutInfo->objectName()).arg(lsColor.at(2));
                    pLayoutInfo->setStyleSheet(strSheet);
                    m_lsPropertyItem.append(pLayoutInfo);
                    QHBoxLayout* ly = new QHBoxLayout(pLayoutInfo);
                    ly->setContentsMargins(6, 2, 3, 2);
                    pLayoutInfo->setLayout(ly);
                    QLabel* pLblName = new QLabel(pLayoutInfo);
                    pLblName->setText("宽,高,水平策略,垂直策略");
                    pLblName->setFixedWidth(145);
                    ly->addWidget(pLblName);
                    QLineEdit* p = new QLineEdit(pLayoutInfo);
                    p->setText(size2String(pSpacerItem->sizeHint()));
                    ly->addWidget(p);

                    QComboBox* comBoxH = new QComboBox(pLayoutInfo);
                    QComboBox* comBoxV = new QComboBox(pLayoutInfo);
                    comBoxH->setMaximumWidth(90);
                    comBoxV->setMaximumWidth(90);
                    QMetaEnum metaEnum = QMetaEnum::fromType<QSizePolicy::Policy>();
                    QStringList lsKeys;
                    for (int i = 0; i < metaEnum.keyCount(); i++)
                    {
                        lsKeys.append(metaEnum.key(i));
                    }
                    comBoxH->addItems(lsKeys);
                    comBoxV->addItems(lsKeys);
                    comBoxH->setCurrentText(metaEnum.valueToKey(pSpacerItem->sizePolicy().horizontalPolicy()));
                    comBoxV->setCurrentText(metaEnum.valueToKey(pSpacerItem->sizePolicy().verticalPolicy()));

                    ly->addWidget(comBoxH);
                    ly->addWidget(comBoxV);

                    static auto refreshSpacerItem = [layout, pW, p, comBoxH, comBoxV, pSpacerItem, metaEnum, this]()
                    {
                        m_mapObjectLayout.insert(pW, layout);
                        if (!m_mapObjectValues.contains(pW))
                        {
                            m_mapObjectValues.insert(pW, QList<ST_PROPERTY>());
                        }
                        QSize size = string2Size(p->text().trimmed());
                        pSpacerItem->changeSize(size.width(), size.height(),
                                                (QSizePolicy::Policy)metaEnum.keyToValue(comBoxH->currentText().toLocal8Bit().data()),
                                                (QSizePolicy::Policy)metaEnum.keyToValue(comBoxV->currentText().toLocal8Bit().data()));
                        layout->invalidate();
                    };

                    connect(p, &QLineEdit::editingFinished, layout, refreshSpacerItem, Qt::UniqueConnection);
                    connect(comBoxH, &QComboBox::currentTextChanged, layout, refreshSpacerItem, Qt::UniqueConnection);
                    connect(comBoxV, &QComboBox::currentTextChanged, layout, refreshSpacerItem, Qt::UniqueConnection);

                    ui->vlayPropery->addWidget(pLayoutInfo);

                }

            }
        }
    }

    show();
}

void CPropertyPanel::showEvent(QShowEvent* event)
{
    if (!m_hasMove)
    {
        //默认右上角
        QRect rect = qApp->desktop()->screenGeometry();
        move(rect.width() - width() - 30, 100);
        m_hasMove = true;
    }
    QWidget::showEvent(event);
    Q_EMIT visibleChanged(isVisible());
}

void CPropertyPanel::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    Q_EMIT visibleChanged(isVisible());
}

void CPropertyPanel::changedInfo(QWidget *pW, const QString &strName, const QVariant value)
{
    if (pW == Q_NULLPTR)
    {
        return;
    }

    int nIndex = pW->metaObject()->indexOfProperty(strName.toLatin1());
    if (nIndex < 0)
    {
        return;
    }
    QMetaProperty mProperty = pW->metaObject()->property(nIndex);
    if (!mProperty.isValid())
    {
        return;
    }
    QVariant oldValue = mProperty.read(pW);
    bool bRet = mProperty.write(pW, value);
    mProperty.notifySignal();

    if (!bRet)
    {
        return;
    }

    QList<ST_PROPERTY> lsPropertys;
    if (m_mapObjectValues.contains(pW))
    {
        lsPropertys = m_mapObjectValues.value(pW);
    }

    ST_PROPERTY stInfo;
    for (int i = 0; i < lsPropertys.length(); ++i) {
        if (lsPropertys.at(i).strPropertyName == strName)
        {
            stInfo = lsPropertys.at(i);
            if (mProperty.isEnumType())
            {
                stInfo.stValues.newValue = mProperty.enumerator().valueToKey(value.toInt());
            }
            else
            {
                stInfo.stValues.newValue = value;
            }

            lsPropertys.replace(i, stInfo);
            break;
        }
    }
    if (stInfo.strPropertyName.isEmpty())
    {
        stInfo.strPropertyName = strName;
        if (mProperty.isEnumType())
        {
            stInfo.stValues.newValue = mProperty.enumerator().valueToKey(value.toInt());
            stInfo.stValues.oldValue = mProperty.enumerator().valueToKey(oldValue.toInt());
        }
        else
        {
            stInfo.stValues.newValue = value;
            stInfo.stValues.oldValue = oldValue;
        }

        lsPropertys.append(stInfo);
    }

    m_mapObjectValues.insert(pW, lsPropertys);

    printLog();
}

void CPropertyPanel::printLog()
{
    // Log
    ui->textBrowser->clear();
    QMapIterator<void*, QList<ST_PROPERTY>> i(m_mapObjectValues);
    while (i.hasNext())
    {
        i.next();
        QList<ST_PROPERTY> lsPropertys = i.value();
        for (int index = 0; index < lsPropertys.length(); ++index)
        {
            ST_PROPERTY stP = lsPropertys.at(index);
            QString strFisrtUp = stP.strPropertyName;
            if (strFisrtUp.length() > 0)
            {
                strFisrtUp = strFisrtUp.at(0).toUpper() + strFisrtUp.mid(1);
            }
            QString strTypeInfo;

            if (stP.stValues.newValue.type() == QVariant::Rect)
            {
                strTypeInfo = rect2String(stP.stValues.newValue.toRect());
            }
            else if (stP.stValues.newValue.type() == QVariant::RectF)
            {
                strTypeInfo = rectF2String(stP.stValues.newValue.toRectF());
            }
            else if (stP.stValues.newValue.type() == QVariant::Point)
            {
                strTypeInfo = point2String(stP.stValues.newValue.toPoint());
            }
            else if (stP.stValues.newValue.type() == QVariant::PointF)
            {
                strTypeInfo = pointF2String(stP.stValues.newValue.toPointF());
            }
            else if (stP.stValues.newValue.type() == QVariant::Size || stP.stValues.newValue.type() == QVariant::SizeF)
            {
                strTypeInfo = size2String(stP.stValues.newValue.toSize());
            }
            else
            {
                strTypeInfo = QString("\"%1\"").arg(stP.stValues.newValue.toString());
            }
            QString strClassInfo;
            QObject* pObject = static_cast<QObject*>(i.key());
            if (pObject != Q_NULLPTR)
            {
                strClassInfo = pObject->objectName();
                if (strClassInfo.isEmpty())
                {
                    strClassInfo = pObject->metaObject()->className();
                }
            }
            QString strInfo = strClassInfo + "->" + "set" + strFisrtUp + QString("(%1)").arg(strTypeInfo);
            ui->textBrowser->append(strInfo);
        }

        if (m_mapObjectLayout.contains(i.key()))
        {
            QLayout* layout = m_mapObjectLayout.value(i.key());
            if (layout == Q_NULLPTR)
            {
                return;
            }
            QString strInfo = layout->objectName() + "->" + "setContentsMargins" + QString("(%1)").arg(Margins2String(layout->contentsMargins()));
            ui->textBrowser->append(strInfo);

            strInfo = layout->objectName() + "->" + "setSpacing" + QString("(%1)").arg(QString::number(layout->spacing()));
            ui->textBrowser->append(strInfo);
        }
        ui->textBrowser->append("");
    }
}

void CPropertyPanel::on_btnAdjustSize_clicked()
{
    if (m_pTarget == Q_NULLPTR)
    {
        return;
    }

    //自适应大小
    while (m_pTarget != Q_NULLPTR) {
        m_pTarget->adjustSize();
        m_pTarget = m_pTarget->parentWidget();
    }
}

void CPropertyPanel::initWidget()
{
    setStyleSheet("");
    setWindowTitle(QString("属性面板"));

    //setAttribute(Qt::WA_TranslucentBackground);
    //设置窗口模态,消息就不会被堵塞了!
    setWindowModality(Qt::WindowModal);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    //setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowOpacity(1.0);
    ui->btnAdjustSize->hide();

    //还原自己
    QString strSelfQss = appContext->getSelfQssInfo();
    this->setStyleSheet(strSelfQss);
    this->style()->unpolish(this);
    this->style()->polish(this);
    connect(appContext, &AppContext::updateQssInfo, this, [this]()
    {
        QString strSelfQss = appContext->getSelfQssInfo();
        this->setStyleSheet(strSelfQss);
        this->style()->unpolish(this);
        this->style()->polish(this);
    });

    auto refreshSettings = [this]()
    {
        ST_CONFIG config = appContext->settings();
        setWindowOpacity(config.propertyOpacity);
    };
    connect(appContext, &AppContext::updateSettings, this, refreshSettings);
    refreshSettings();

    auto expand = [this](bool visible)
    {
        ui->btnToggleCodeExample->setChecked(visible);
        ui->textBrowser->setVisible(visible);
        if (visible)
        {
            ui->splitter->setSizes(QList<int>() << height() - 300 << 300);
        }
        else
        {
            ui->splitter->setSizes(QList<int>() << height() - ui->btnToggleCodeExample->height() << ui->btnToggleCodeExample->height());
        }
    };
    connect(ui->btnToggleCodeExample, &QPushButton::toggled, this, expand);
    expand(false);
}
