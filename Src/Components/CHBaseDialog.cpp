/**
  * 基础对话框类
  * @author   	ch
  *
*/

#include <QWindow>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <QCloseEvent>
#include <QDesktopWidget>
#include "CHBaseDialog.h"
#include "ui_CHBaseDialog.h"

#define M_CONFIG_SHADOW_WIDTH 0

CHBaseDialog::CHBaseDialog(QWidget* parent)
    : QDialog(parent)
{
    ui = new Ui::CHBaseDialog();
    ui->setupUi(this);

    //初始化变量
    m_bMinimizable = false;
    m_bMaximizable = false;
    m_bClosable = false;
    m_bMovable = true;
    m_bResizable = false;
    m_bShadowEffectVisible = false;
    m_nResizeWidth = M_CONFIG_SHADOW_WIDTH;
    m_strMaxStyleSheet = QString("QPushButton#btnDlgMax {background-color: transparent; background-image: url(:/Image/Dialog/弹框小按钮_最大化_normal.png);}"
        "QPushButton#btnDlgMax:hover {background-color: #1168D9;}"
        "QPushButton#btnDlgMax:pressed {background-color: #105DC2;}");
    m_strRestoreStyleSheet = QString("QPushButton#btnDlgMax {background-color: transparent; background-image: url(:/Image/Dialog/弹框小按钮_还原_normal.png);}"
        "QPushButton#btnDlgMax:hover {background-color: #1168D9;}"
        "QPushButton#btnDlgMax:pressed {background-color: #105DC2;}");

    //初始化界面
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    installEventFilter(this);
    setShadowEffectVisible(false);
    initTitleBar();
}

CHBaseDialog::~CHBaseDialog()
{
    delete ui;
}

//初始化标题栏
void CHBaseDialog::initTitleBar()
{
    //按钮
    ui->btnDlgMin->setAutoDefault(false);
    ui->btnDlgMax->setAutoDefault(false);
    ui->btnDlgClose->setAutoDefault(false);
    connect(ui->btnDlgMin, SIGNAL(clicked()), this, SLOT(minimizeDialog()));
    connect(ui->btnDlgMax, SIGNAL(clicked()), this, SLOT(maximizeDialog()));
    connect(ui->btnDlgClose, SIGNAL(clicked()), this, SLOT(closeDialog()));

    //默认只可关闭和移动
    setClosable(true);
    setMovable(true);
    setResizable(false);
    setMinimizable(false);
    setMaximizable(false);
}

//设置主窗口对象名称
void CHBaseDialog::setMainWidgetObjectName(const QString& strName)
{
    ui->wDlgMain->setObjectName(strName);
}

//设置主背景对象名称
void CHBaseDialog::setMainBackgroundObjectName(const QString& strName)
{
    ui->frmDlgBackground->setObjectName(strName);
}

//设置标题栏对象名称
void CHBaseDialog::setTitleBarObjectName(const QString& strName)
{
    ui->frmDlgTitleBar->setObjectName(strName);
}

//设置标题标签对象名称
void CHBaseDialog::setTitleLabelObjectName(const QString& strName)
{
    ui->lblDlgTitle->setObjectName(strName);
}

//设置功能区对象名称
void CHBaseDialog::setFunctionAreaObjectName(const QString& strName)
{
    ui->frmDlgFunctionArea->setObjectName(strName);
}

//设置最小化按钮对象名称
void CHBaseDialog::setMinButtonObjectName(const QString& strName)
{
    ui->btnDlgMin->setObjectName(strName);
}

//设置最大化按钮对象名称
void CHBaseDialog::setMaxButtonObjectName(const QString& strName)
{
    ui->btnDlgMax->setObjectName(strName);
}

//设置关闭按钮对象名称
void CHBaseDialog::setCloseButtonObjectName(const QString& strName)
{
    ui->btnDlgClose->setObjectName(strName);
}

//设置最大化样式表
void CHBaseDialog::setMaxStyleSheet(const QString& strStyleSheet)
{
    m_strMaxStyleSheet = strStyleSheet;
}

//设置还原样式表
void CHBaseDialog::setRestoreStyleSheet(const QString& strStyleSheet)
{
    m_strRestoreStyleSheet = strStyleSheet;
}

//设置标题
void CHBaseDialog::setTitle(const QString& strTitle)
{
    setWindowTitle(strTitle);
    ui->lblDlgTitle->setText(strTitle);
}

//获取标题
QString CHBaseDialog::getTitle() const
{
    return ui->lblDlgTitle->text();
}

//设置是否可见
void CHBaseDialog::setVisible(bool bVisible)
{
    QDialog::setVisible(bVisible);

    if (!bVisible)
    {
        Q_EMIT hidden();
    }
}

//设置是否可最小化
void CHBaseDialog::setMinimizable(bool bMinimizable)
{
    m_bMinimizable = bMinimizable;
    ui->btnDlgMin->setVisible(bMinimizable);
}

//获取是否可最小化
bool CHBaseDialog::isMinimizable() const
{
    return m_bMinimizable;
}

//设置是否可最大化
void CHBaseDialog::setMaximizable(bool bMaximizable)
{
    m_bMaximizable = bMaximizable;
    ui->btnDlgMax->setVisible(bMaximizable);
}

//获取是否可最大化
bool CHBaseDialog::isMaximizable() const
{
    return m_bMaximizable;
}

//设置是否可关闭
void CHBaseDialog::setClosable(bool bClosable)
{
    m_bClosable = bClosable;
    ui->btnDlgClose->setVisible(bClosable);
}

//获取是否可关闭
bool CHBaseDialog::isClosable() const
{
    return m_bClosable;
}

//设置是否可移动
void CHBaseDialog::setMovable(bool bMovable)
{
    m_bMovable = bMovable;
}

//获取是否可移动
bool CHBaseDialog::isMovable() const
{
    return m_bMovable;
}

//设置是否可调整大小
void CHBaseDialog::setResizable(bool bResizable)
{
    m_bResizable = bResizable;
}

//获取是否可调整大小
bool CHBaseDialog::isResizable() const
{
    return m_bResizable;
}

//设置调整大小宽度
void CHBaseDialog::setResizeWidth(int nWidth)
{
    m_nResizeWidth = nWidth;
}

//获取调整大小宽度
int CHBaseDialog::getResizeWidth() const
{
    return m_nResizeWidth;
}

//设置是否显示阴影效果
void CHBaseDialog::setShadowEffectVisible(bool bVisible)
{
    m_bShadowEffectVisible = bVisible;
    ui->vlayDlgMain->setMargin(bVisible ? M_CONFIG_SHADOW_WIDTH : 0);
}

//获取是否显示阴影效果
bool CHBaseDialog::isShadowEffectVisible() const
{
    return m_bShadowEffectVisible;
}

//获取阴影效果宽度
int CHBaseDialog::getShadowEffectWidth() const
{
    return ui->vlayDlgMain->margin();
}

//获取功能窗口
QWidget* CHBaseDialog::getFunctionWidget() const
{
    return ui->wDlgFunction;
}

//设置功能窗口大小
void CHBaseDialog::setFunctionWidgetSize(int nWidth, int nHeight)
{
    ui->wDlgFunction->setFixedSize(nWidth, nHeight);
    adjustSize();
}

//视觉大小转为真实大小
int CHBaseDialog::realSizeFromVisualSize(int nSize)
{
    return nSize + getShadowEffectWidth() * 2;
}

//设置固定的视觉大小
void CHBaseDialog::setFixedVisualSize(int nWidth, int nHeight)
{
    setFixedSize(realSizeFromVisualSize(nWidth), realSizeFromVisualSize(nHeight));
}

//设置最小的视觉大小
void CHBaseDialog::setMinimumVisualSize(int nWidth, int nHeight)
{
    setMinimumSize(realSizeFromVisualSize(nWidth), realSizeFromVisualSize(nHeight));
}

//调整视觉大小
void CHBaseDialog::adjustVisualSize(int nWidth, int nHeight)
{
    setFixedWidth(realSizeFromVisualSize(nWidth));

    if (nHeight < 0)
    {
        adjustSize();
        ui->frmDlgFunctionArea->setFixedHeight(ui->vlayDlgFunctionArea->sizeHint().height());
        ui->frmDlgBackground->setFixedHeight(ui->vlayDlgBackground->sizeHint().height());
        ui->wDlgMain->setFixedHeight(ui->vlayDlgMain->sizeHint().height());
        setFixedHeight(ui->vlayBaseDialog->sizeHint().height());
    }
    else
    {
        setFixedHeight(realSizeFromVisualSize(nHeight));
    }
}

//调整小尺寸视觉大小
void CHBaseDialog::adjustMinVisualSize(int nHeight)
{
    adjustVisualSize(376, nHeight);
}

//调整中尺寸视觉大小
void CHBaseDialog::adjustMidVisualSize(int nHeight)
{
    adjustVisualSize(600, nHeight);
}

//调整大尺寸视觉大小
void CHBaseDialog::adjustMaxVisualSize(int nHeight)
{
    adjustVisualSize(1080, nHeight);
}

//最小化对话框
void CHBaseDialog::minimizeDialog()
{
    showMinimized();
}

//最大化对话框
void CHBaseDialog::maximizeDialog()
{
    if (isMaximized())
    {
        showNormal();
    }
    else
    {
        showMaximized();
    }
}

//关闭对话框
void CHBaseDialog::closeDialog()
{
    reject();

    if (sender() != Q_NULLPTR)
    {
        QApplication::postEvent(sender(), new QEvent(QEvent::Leave), Qt::LowEventPriority);
    }
}

//调整位置
void CHBaseDialog::adjustPosition(QWidget* parent)
{
    QPoint p(0, 0);
    int nExtraW = 0;
    int nExtraH = 0;
    int nScrn = 0;

    //修改此值可调整对话框弹出位置，默认为居中
    int nExtraMaxW = 0;
    int nExtraMaxH = 0;

    if (parent != Q_NULLPTR)
    {
        parent = parent->window();
    }

    QRect rectDesktop;
    if (parent != Q_NULLPTR)
    {
        nScrn = QApplication::desktop()->screenNumber(parent);
    }
    else if (QApplication::desktop()->isVirtualDesktop())
    {
        nScrn = QApplication::desktop()->screenNumber(QCursor::pos());
    }
    else
    {
        nScrn = QApplication::desktop()->screenNumber(this);
    }
    rectDesktop = QApplication::desktop()->availableGeometry(nScrn);

    QWidgetList lstWidget = QApplication::topLevelWidgets();
    for (int i = 0; (0 == nExtraW || 0 == nExtraH) && i < lstWidget.size(); ++i)
    {
        QWidget* pCurrent = lstWidget.at(i);
        if (pCurrent->isVisible())
        {
            int nFrameW = pCurrent->geometry().x() - pCurrent->x();
            int nFrameH = pCurrent->geometry().y() - pCurrent->y();

            nExtraW = qMax(nExtraW, nFrameW);
            nExtraH = qMax(nExtraH, nFrameH);
        }
    }

    //sanity check for decoration frames. With embedding, we might get extraordinary values
    if (0 == nExtraW ||
        0 == nExtraH ||
        nExtraW >= nExtraMaxW ||
        nExtraH >= nExtraMaxH)
    {
        nExtraW = nExtraMaxW;
        nExtraH = nExtraMaxH;
    }

    if (parent != Q_NULLPTR)
    {
        //Use pos() if the widget is embedded into a native window
        QPoint pp;
        if (parent->windowHandle() &&
            parent->windowHandle()->property("_q_embedded_native_parent_handle").value<WId>())
        {
            pp = parent->pos();
        }
        else
        {
            pp = parent->mapToGlobal(QPoint(0, 0));
        }
        p = QPoint(pp.x() + parent->width() / 2, pp.y() + parent->height() / 2);
    }
    else
    {
        //p = middle of the desktop
        p = QPoint(rectDesktop.x() + rectDesktop.width() / 2, rectDesktop.y() + rectDesktop.height() / 2);
    }

    //p = origin of this
    p = QPoint(p.x() - width() / 2 - nExtraW, p.y() - height() / 2 - nExtraH);
    if (p.x() + nExtraW + width() > rectDesktop.x() + rectDesktop.width())
    {
        p.setX(rectDesktop.x() + rectDesktop.width() - width() - nExtraW);
    }
    if (p.x() < rectDesktop.x())
    {
        p.setX(rectDesktop.x());
    }
    if (p.y() + nExtraH + height() > rectDesktop.y() + rectDesktop.height())
    {
        p.setY(rectDesktop.y() + rectDesktop.height() - height() - nExtraH);
    }
    if (p.y() < rectDesktop.y())
    {
        p.setY(rectDesktop.y());
    }

    //QTBUG-52735: Manually set the correct target screen since scaling in a
    //subsequent call to QWindow::resize() may otherwise use the wrong factor
    //if the screen changed notification is still in an event queue.
    if (nScrn >= 0)
    {
        if (QWindow* pWindow = windowHandle())
        {
            pWindow->setScreen(QGuiApplication::screens().at(nScrn));
        }
    }

    move(p);
}

//显示事件
void CHBaseDialog::showEvent(QShowEvent* event)
{
    setAttribute(Qt::WA_Mapped);

    if (!event->spontaneous() &&
        !testAttribute(Qt::WA_Moved))
    {
        Qt::WindowStates state = windowState();
        adjustPosition(parentWidget());
        setAttribute(Qt::WA_Moved, false);

        if (state != windowState())
        {
            setWindowState(state);
        }

        Q_EMIT started();
    }
}

//关闭事件
void CHBaseDialog::closeEvent(QCloseEvent* event)
{
    if (isVisible())
    {
        if (isClosable())
        {
            closeDialog();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
}

//按键事件
void CHBaseDialog::keyPressEvent(QKeyEvent* event)
{
    //如果不可关闭，则按下“ESC”键时不关闭对话框
    if (event->matches(QKeySequence::Cancel))
    {
        if (isClosable())
        {
            closeDialog();
        }

        event->ignore();
        return;
    }

    QDialog::keyPressEvent(event);
}

#ifdef Q_OS_WIN
//本地事件
bool CHBaseDialog::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);

    MSG* pMsg = static_cast<MSG*>(message);
    switch (pMsg->message)
    {
    case WM_NCHITTEST:
    {
        //获取指针位置
        QPoint posCursor = mapFromGlobal(QCursor::pos());

        //调整大小处理
        if (isResizable() &&
            !isMaximized())
        {
            int nWidth = width();
            int nHeight = height();
            QRect rectTop = QRect(m_nResizeWidth, 0, nWidth - m_nResizeWidth * 2, m_nResizeWidth);
            QRect rectBottom = QRect(m_nResizeWidth, nHeight - m_nResizeWidth, nWidth - m_nResizeWidth * 2, m_nResizeWidth);
            QRect rectLeft = QRect(0, m_nResizeWidth, m_nResizeWidth, nHeight - m_nResizeWidth * 2);
            QRect rectRight = QRect(nWidth - m_nResizeWidth, m_nResizeWidth, m_nResizeWidth, nHeight - m_nResizeWidth * 2);
            QRect rectTopLeft = QRect(0, 0, m_nResizeWidth, m_nResizeWidth);
            QRect rectTopRight = QRect(nWidth - m_nResizeWidth, 0, m_nResizeWidth, m_nResizeWidth);
            QRect rectBottomLeft = QRect(0, nHeight - m_nResizeWidth, m_nResizeWidth, m_nResizeWidth);
            QRect rectBottomRight = QRect(nWidth - m_nResizeWidth, nHeight - m_nResizeWidth, m_nResizeWidth, m_nResizeWidth);

            if (rectTop.contains(posCursor))
            {
                *result = HTTOP;
                return true;
            }
            else if (rectBottom.contains(posCursor))
            {
                *result = HTBOTTOM;
                return true;
            }
            else if (rectLeft.contains(posCursor))
            {
                *result = HTLEFT;
                return true;
            }
            else if (rectRight.contains(posCursor))
            {
                *result = HTRIGHT;
                return true;
            }
            else if (rectTopLeft.contains(posCursor))
            {
                *result = HTTOPLEFT;
                return true;
            }
            else if (rectTopRight.contains(posCursor))
            {
                *result = HTTOPRIGHT;
                return true;
            }
            else if (rectBottomLeft.contains(posCursor))
            {
                *result = HTBOTTOMLEFT;
                return true;
            }
            else if (rectBottomRight.contains(posCursor))
            {
                *result = HTBOTTOMRIGHT;
                return true;
            }
        }

        //标题栏处理
        if (isMovable())
        {
            QWidget* pWidget = childAt(posCursor);
            if (pWidget == ui->lblDlgTitle ||
                pWidget == ui->frmDlgTitleBar)
            {
                *result = HTCAPTION;
                return true;
            }
        }
    }
    break;

    case WM_NCLBUTTONDBLCLK:
    {
        //标题栏双击处理
        if (isMaximizable())
        {
            maximizeDialog();
        }
        return true;
    }
    break;

    default:
        break;
    }

    return QDialog::nativeEvent(eventType, message, result);
}
#else
bool CHBaseDialog::eventFilter(QObject* pWatched, QEvent* pEvent)
{
    // 标题栏处理
        static bool bTitlePressed = false;
        static bool bTitleMovable = false;
        static QPoint ptTitlePressedPos;
        static QSize szTitlePressedSize;
        if (isMovable()
            /*&& (pWatched == ui->frmDlgTitleBar || pWatched == ui->lblDlgIcon || pWatched == ui->lblDlgTitle)*/)
        {
            if (pEvent->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
                if (pMouseEvent->button() == Qt::LeftButton)
                {
                    bTitlePressed = true;
                    ptTitlePressedPos = pMouseEvent->pos();
                    szTitlePressedSize = size();
                }
            }
            else if (pEvent->type() == QEvent::MouseMove)
            {
                QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
                if (bTitlePressed)
                {
                    if (bTitleMovable)
                    {
                        if (szTitlePressedSize != size())
                        {
                            int nWidthMaximized = szTitlePressedSize.width();
                            int nWidthNormal = size().width();
                            int nPosX = ptTitlePressedPos.x();
                            int nPosY = ptTitlePressedPos.y();
                            if (nPosX < (nWidthMaximized / 2))
                            {
                                if (nPosX > (nWidthNormal / 2))
                                {
                                    nPosX = nWidthNormal / 2;
                                }
                            }
                            else
                            {
                                if ((nWidthMaximized - nPosX) > (nWidthNormal / 2))
                                {
                                    nPosX = nWidthNormal / 2;
                                }
                                else
                                {
                                    nPosX = nWidthNormal - (nWidthMaximized - nPosX);
                                }
                            }
                            ptTitlePressedPos = QPoint(nPosX, nPosY);
                        }

                        move(pMouseEvent->globalPos() - ptTitlePressedPos);
                    }
                    else
                    {
                        QPoint ptDistance = pMouseEvent->pos() - ptTitlePressedPos;
                        if (ptDistance.manhattanLength() >= QApplication::startDragDistance())
                        {
                            if (isResizable()
                                && isMaximized())
                            {
                                maximizeDialog();
                            }

                            bTitleMovable = true;
                        }
                    }
                }
            }
            else if (pEvent->type() == QEvent::MouseButtonRelease)
            {
                bTitlePressed = false;
                bTitleMovable = false;
            }
            else if (pEvent->type() == QEvent::MouseButtonDblClick)
            {
                if (isResizable())
                {
                    maximizeDialog();
                }
            }
        }
    return QDialog::eventFilter(pWatched, pEvent);
}

#endif

//改变事件
void CHBaseDialog::changeEvent(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowStateChange:
    {
        if (isMaximized())
        {
            ui->vlayDlgMain->setMargin(0);
            ui->btnDlgMax->setStyleSheet(m_strRestoreStyleSheet);
        }
        else
        {
            ui->vlayDlgMain->setMargin(m_bShadowEffectVisible ? M_CONFIG_SHADOW_WIDTH : 0);
            ui->btnDlgMax->setStyleSheet(m_strMaxStyleSheet);
        }
    }
    break;

    default:
        break;
    }

    QDialog::changeEvent(event);
}
