#include <QStyle>
#include <QTimer>
#include <QWidget>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include "GlobalSelector.h"
#include "PropertyPanel.h"
#include <QMenu>
#include <QDebug>
#include "Resources.h"
#include "AppContext.h"
#include <QLabel>
#include <QElapsedTimer>
#include <QPainter>
#include "ObjectInfoMenu.h"

class CGlobalSelectorPrivate
{
public:
    CGlobalSelectorPrivate(QObject* parent)
    {
        m_q = parent;
        //Hight
        m_pHighlight = new QWidget;
        m_pLastObj = Q_NULLPTR;
        m_pBg = new QWidget(m_pHighlight);
        m_pBg->setObjectName("wSelectorBg");
        QHBoxLayout* layout = new QHBoxLayout(m_pHighlight);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_pBg);
        m_pHighlight->setAttribute(Qt::WA_TranslucentBackground);
        m_pHighlight->setWindowFlags(Qt::Tool |
                                     Qt::FramelessWindowHint |
                                     Qt::WindowStaysOnTopHint |
                                     Qt::X11BypassWindowManagerHint |
                                     Qt::WindowTransparentForInput);
        m_pMoveTimer = new QTimer(parent);
        m_pMoveTimer->setInterval(100);
        m_pMoveTimer->start();

        m_pObjectMenu = new ObjectInfoMenu(qobject_cast<QWidget*>(parent));
        m_pObjectMenu->setObjectName("objectMenu");

        m_bSelectEnabled = false;
        m_fpsTimer.start();

        m_updateTimer = new QTimer(parent);
        QObject::connect(m_updateTimer, &QTimer::timeout, parent, [=](){
            if (m_fpsTimer.elapsed() >= 1000) {
                m_fpsTimer.restart();
                updateFps();
                m_mainFpsCount = 0;
                m_selectFpsCount = 0;
            }
        });
        m_updateTimer->setInterval(10);
        m_updateTimer->start();
    }

    void updateFps() {
        appContext->setFpsValue(m_mainFpsCount, m_selectFpsCount);
    }

    ~CGlobalSelectorPrivate()
    {
        m_pHighlight->deleteLater();
        m_pObjectMenu->deleteLater();
    }

public:
    QObject* m_q;
    QWidget* m_pHighlight;
    QWidget* m_pBg;
    QTimer* m_pMoveTimer;
    QObject* m_pLastObj;
    ObjectInfoMenu* m_pObjectMenu;
    QList<QWidget*> m_widgets;
    bool m_bSelectEnabled;

    QElapsedTimer m_fpsTimer;
    int m_mainFpsCount = 0;
    int m_selectFpsCount = 0;
    QTimer* m_updateTimer;
};

CGlobalSelector::CGlobalSelector(QObject *parent) : QObject(parent)
{
    m_p = new CGlobalSelectorPrivate(qobject_cast<QWidget*>(parent));
    connect(m_p->m_pMoveTimer, &QTimer::timeout, this, [this]()
    {
        if (m_p->m_pLastObj == Q_NULLPTR)
        {
            m_p->m_pHighlight->hide();
            return;
        }

        if (m_p->m_bSelectEnabled || m_p->m_pObjectMenu->isVisible()) {
            return;
        }

        QWidget* pWidget = qobject_cast<QWidget*>(m_p->m_pLastObj);
        if (pWidget != Q_NULLPTR)
        {
            QWidget* pRootParent = pWidget;
            while (pRootParent->parentWidget() != Q_NULLPTR)
            {
                pRootParent = pRootParent->parentWidget();
            }

            if (pWidget->isVisible() && !pRootParent->isMinimized() && appContext->settings().objectBorderVisible)
            {
                m_p->m_pHighlight->move(pWidget->mapToGlobal(QPoint(0, 0)));
                m_p->m_pHighlight->setFixedSize(pWidget->size());
                m_p->m_pHighlight->show();
            }
            else
            {
                m_p->m_pHighlight->hide();
            }
        }
    });

    connect(m_p->m_pObjectMenu, &ObjectInfoMenu::selectedWidget, this, [this](QPointer<QWidget> widgetItem)
    {
        if (!widgetItem.isNull())
        {
            m_p->m_pLastObj = widgetItem;
            m_p->m_pHighlight->hide();
            //QPoint ptAll = widgetItem->mapFromGlobal(QPoint(0, 0));
            //qDebug(QString(QString("x:%1 y:%2").arg(ptAll.x()).arg(ptAll.y())).toLatin1());
            m_p->m_pHighlight->move(widgetItem->mapToGlobal(QPoint(0, 0)));
            m_p->m_pHighlight->setFixedSize(widgetItem->size());

            m_p->m_pHighlight->show();
            Q_EMIT selectedWidget(widgetItem);
        }
    });

    connect(m_p->m_pObjectMenu, &ObjectInfoMenu::hoveredWidget, this, [this](QPointer<QWidget> widgetItem)
    {
        if (!widgetItem.isNull())
        {
            m_p->m_pMoveTimer->start();
            m_p->m_pLastObj = widgetItem;
            m_p->m_pHighlight->hide();
            m_p->m_pHighlight->move(widgetItem->mapToGlobal(QPoint(0, 0)));
            m_p->m_pHighlight->setFixedSize(widgetItem->size());

            m_p->m_pHighlight->show();
        }
    });

    qApp->installEventFilter(this);

    auto refreshSettings = [this]()
    {
        ST_CONFIG config = appContext->settings();
        m_p->m_pBg->setStyleSheet(QString("QWidget{border: %1px solid %2; background-color: transparent;}")
                                  .arg(config.objectBorderWidth).arg(config.objectBorderColor.name()));
        m_p->m_pBg->style()->unpolish(m_p->m_pBg);
        m_p->m_pBg->style()->polish(m_p->m_pBg);
    };
    connect(appContext, &AppContext::updateSettings, this, refreshSettings);
    refreshSettings();
}

CGlobalSelector::~CGlobalSelector()
{
    delete m_p;
}

void CGlobalSelector::setNotHandleEventList(const QList<QWidget*> widgets)
{
    m_p->m_widgets = widgets;
}

void CGlobalSelector::setSelectState(const bool bEnabled)
{
    if (m_p->m_bSelectEnabled == bEnabled)
    {
        return;
    }

    m_p->m_bSelectEnabled = bEnabled;
    if (bEnabled)
    {
        qGuiApp->setOverrideCursor(Qt::CrossCursor);
        Q_EMIT selectStateChanged(true);
    }
    else
    {
        qGuiApp->restoreOverrideCursor();
        Q_EMIT selectStateChanged(false);
    }
}

bool CGlobalSelector::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(event);
    do
    {
        if (pMouseEvent == Q_NULLPTR)
        {
            break;
        }

        if (((pMouseEvent->type() == QEvent::MouseButtonRelease ||
             pMouseEvent->type() == QEvent::MouseButtonPress ||
             pMouseEvent->type() == QEvent::MouseMove ||
             pMouseEvent->type() == QEvent::MouseButtonDblClick ||
             pMouseEvent->type() == QEvent::MouseTrackingChange) &&
             pMouseEvent->button() == Qt::LeftButton &&
             m_p->m_bSelectEnabled))
        {
            setSelectState(true);

            //避免双击造成多次显示
            if (pMouseEvent->type() == QEvent::MouseButtonRelease)
            {
                setSelectState(false);
                return true;
            }

            QWidget* pWidget = static_cast<QWidget*>(obj);
            if (pWidget == Q_NULLPTR)
            {
                setSelectState(false);
                return true;
            }

            QString strClassName = QString::fromLatin1(pWidget->metaObject()->className());
            //屏蔽系统的,系统的获取不到子Widget
            if (pWidget->objectName() == "MainWindowWindow" || strClassName == "QWidgetWindow")
            {
                return false;
            }

            //当前正在选择状态禁止操作
            if (m_p->m_pObjectMenu->count() > 0)
            {
                setSelectState(false);
                return true;
            }

            setSelectState(false);

            QStringList lsObjects;
            QList<QWidget*> lsWidgets;
            while (pWidget)
            {
                QString strClassName = QString::fromLatin1(pWidget->metaObject()->className());
                lsWidgets.append(pWidget);
                QString strObjectName = pWidget->objectName();
                if (strObjectName.isEmpty())
                {
                    strObjectName = tr("未定义");
                }
                QString strClassInfo = QString("%1(%2)").arg(strObjectName).arg(strClassName);
                lsObjects.append(strClassInfo);
                //qDebug(strClassInfo.toLatin1());
                pWidget = pWidget->parentWidget();

                //面板自身屏蔽了
                if (m_p->m_widgets.contains(pWidget))
                {
                    return true;
                }
            }

            for (int i = 0; i < lsObjects.length(); ++i)
            {
                QWidget* widgetItem = lsWidgets.at(i);
                m_p->m_pObjectMenu->addWidget(widgetItem);
                //释放的时候设置指针
                connect(widgetItem, &QWidget::destroyed, this, [this, widgetItem]()
                {
                    if (m_p->m_pLastObj == widgetItem)
                    {
                        m_p->m_pLastObj = Q_NULLPTR;
                    }
                }, Qt::UniqueConnection);
            }

            if (m_p->m_pObjectMenu->exec(QCursor::pos()) == Q_NULLPTR)
            {
                m_p->m_pHighlight->hide();
                m_p->m_pLastObj = Q_NULLPTR;
            }
            m_p->m_pObjectMenu->clear();
            return true;
        }
    } while (0);

    // FPS
    if (event->type() == QEvent::Paint) {
        QString className = obj->metaObject()->className();
        if (obj->parent() == Q_NULLPTR && (obj->inherits("QMainWindow") || className.contains("MainWindow"))) {
            m_p->m_mainFpsCount++;
        }

        bool isContainObj = false;
        auto objectParent = obj;
        do {
            if (objectParent == m_p->m_pLastObj) {
                isContainObj = true;
                break;
            }
            objectParent = objectParent->parent();
        } while (objectParent);

        if (isContainObj) {
            m_p->m_selectFpsCount++;
        }
    }
    return QObject::eventFilter(obj, event);
}
