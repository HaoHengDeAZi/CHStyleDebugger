/**
  * 消息提示框类
  * @author   	ch
  *
*/

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include "Resources.h"
#include "CHMessageToast.h"
#include "ui_CHMessageToast.h"

CHMessageToast::CHMessageToast(QWidget* parent)
    : QWidget(parent)
    , m_nShowDuration(2000)
    , m_pShowTimer(Q_NULLPTR)
    , m_pShowAnimation(Q_NULLPTR)
{
    ui = new Ui::CHMessageToast();
    ui->setupUi(this);

    //初始化界面
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool |
                   Qt::FramelessWindowHint |
                   Qt::CustomizeWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);
    setWindowOpacity(0);
    initWidget();
}

CHMessageToast::~CHMessageToast()
{
    delete ui;
}

//设置显示持续时间
void CHMessageToast::setShowDuration(int nMSecs)
{
    m_nShowDuration = nMSecs;
}

//获取显示持续时间
int CHMessageToast::getShowDuration() const
{
    return m_nShowDuration;
}

//信息消息框
void CHMessageToast::information(QWidget* pParent, const QString& strText, int nShowDuration)
{
    CHMessageToast* pMessageToast = getMessageToast(pParent);
    if (pMessageToast != Q_NULLPTR)
    {
        pMessageToast->showMessageToast(":/Image/Dialog/消息图标_信息.png", strText, nShowDuration);
    }
}

//警告消息框
void CHMessageToast::warning(QWidget* pParent, const QString& strText, int nShowDuration)
{
    CHMessageToast* pMessageToast = getMessageToast(pParent);
    if (pMessageToast != Q_NULLPTR)
    {
        pMessageToast->showMessageToast(":/Image/Dialog/消息图标_警告.png", strText, nShowDuration);
    }
}

//错误消息框
void CHMessageToast::error(QWidget* pParent, const QString& strText, int nShowDuration)
{
    CHMessageToast* pMessageToast = getMessageToast(pParent);
    if (pMessageToast != Q_NULLPTR)
    {
        pMessageToast->showMessageToast(":/Image/Dialog/消息图标_错误.png", strText, nShowDuration);
    }
}

//成功消息框
void CHMessageToast::success(QWidget* pParent, const QString& strText, int nShowDuration)
{
    CHMessageToast* pMessageToast = getMessageToast(pParent);
    if (pMessageToast != Q_NULLPTR)
    {
        pMessageToast->showMessageToast(":/Image/Dialog/消息图标_成功.png", strText, nShowDuration);
    }
}

//窗口显示事件;
void CHMessageToast::showEvent(QShowEvent* event)
{
    QPoint posGlobal;
    QWidget* pParentWidget = parentWidget();

    if (Q_NULLPTR != pParentWidget)
    {
        posGlobal = pParentWidget->mapToGlobal(QPoint(pParentWidget->width() / 2, pParentWidget->height() / 2));
    }
    else
    {
        posGlobal = QApplication::desktop()->screenGeometry().center();
    }

    move(posGlobal - QPoint(width() / 2, height() / 2));
    return QWidget::showEvent(event);
}

//初始化窗口
void CHMessageToast::initWidget()
{
    //显示定时器
    m_pShowTimer = new QTimer(this);
    m_pShowTimer->setSingleShot(true);
    connect(m_pShowTimer, SIGNAL(timeout()), this, SLOT(startFadeOutAnimation()));

    //显示动画
    m_pShowAnimation = new QPropertyAnimation(this);
    m_pShowAnimation->setTargetObject(this);
    m_pShowAnimation->setPropertyName("windowOpacity");
}

//显示消息提示框
void CHMessageToast::showMessageToast(const QString& strIcon, const QString& strText, int nShowDuration)
{
    //设置消息信息
    ui->lblToastIcon->setPixmap(QPixmap(strIcon));
    ui->lblToastText->setText(strText);
    adjustSize();

    //设置显示持续时间
    setShowDuration(nShowDuration);

    //开始显示
    if (!m_pShowTimer->isActive())
    {
        show();
        startFadeInAnimation();
    }
    m_pShowTimer->start(nShowDuration);
}

//获取消息框
CHMessageToast* CHMessageToast::getMessageToast(QWidget* pParent)
{
    CHMessageToast* pMessageToast = Q_NULLPTR;
    if (pParent != Q_NULLPTR)
    {
        pMessageToast = pParent->findChild<CHMessageToast*>();
    }

    if (Q_NULLPTR == pMessageToast)
    {
        pMessageToast = new CHMessageToast(pParent);
    }

    return pMessageToast;
}

//开始淡入动画
void CHMessageToast::startFadeInAnimation()
{
    disconnect(m_pShowAnimation, SIGNAL(finished()), this, SLOT(close()));

    m_pShowAnimation->stop();
    m_pShowAnimation->setDuration(750);
    m_pShowAnimation->setStartValue(windowOpacity());
    m_pShowAnimation->setEndValue(1);
    m_pShowAnimation->start();
}

//开始淡出动画
void CHMessageToast::startFadeOutAnimation()
{
    connect(m_pShowAnimation, SIGNAL(finished()), this, SLOT(close()));

    m_pShowAnimation->stop();
    m_pShowAnimation->setDuration(750);
    m_pShowAnimation->setStartValue(1);
    m_pShowAnimation->setEndValue(0);
    m_pShowAnimation->start();
}
