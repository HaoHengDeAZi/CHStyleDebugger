#include <QMenu>
#include <QDebug>
#include <QStyle>
#include <QScreen>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include "StyleDebugger.h"
#include "ui_StyleDebugger.h"
#include "AppContext.h"
#include "QssEditor.h"
#include "Resources.h"
#include "PropertyPanel.h"
#include "GlobalSelector.h"
#include "Settings/About.h"
#include "Settings/SoftwareSettings.h"
#include "Settings/QssEditorSettings.h"
#include "Components/CHMessageToast.h"

StyleDebugger::StyleDebugger(QWidget *parent) :
    CHBaseDialog(parent),
    ui(new Ui::StyleDebugger),
    m_hasMove(false),
    m_QssEditorHasMove(false),
    m_pQssEditor(Q_NULLPTR),
    m_pPropertyPanel(Q_NULLPTR),
    m_pGlobalSelector(Q_NULLPTR),
    m_trayIcon(Q_NULLPTR)
{
    ui->setupUi(getFunctionWidget());

    initWidget();
    initSettings();
}

StyleDebugger::~StyleDebugger()
{
    delete ui;
}

void StyleDebugger::toggleQssEditorVisible()
{
    if (appContext->qssConfig().qssFiles.isEmpty())
    {
        QssEditorSettings settings(this);
        settings.exec();
        if (appContext->qssConfig().qssFiles.isEmpty())
        {
            CHMessageToast::warning(this, tr("请添加qss文件再使用"));
            return;
        }
    }
    m_pQssEditor->setVisible(!m_pQssEditor->isVisible());
    ui->btnStyleSheet->setChecked(m_pQssEditor->isVisible());
    if (m_pQssEditor->isVisible())
    {
        m_pQssEditor->activateWindow();

        if (!m_QssEditorHasMove)
        {
            //默认右上角
            QRect rect = qApp->desktop()->screenGeometry();
            m_pQssEditor->move(rect.width() - m_pQssEditor->width(), height());
            m_QssEditorHasMove = true;
        }
    }
}

void StyleDebugger::togglePropertyPanelVisible()
{
    if (m_pPropertyPanel->objectWidget() == Q_NULLPTR)
    {
        CHMessageToast::warning(this, tr("请先选择对象"));
        selectObject(true);
        ui->btnProperty->setChecked(false);
        return;
    }

    m_pPropertyPanel->setVisible(!m_pPropertyPanel->isVisible());
    ui->btnProperty->setChecked(m_pPropertyPanel->isVisible());
    if (m_pPropertyPanel->isVisible())
    {
        m_pPropertyPanel->activateWindow();
    }
}

void StyleDebugger::selectObject(bool alwaysSelect)
{
    if (alwaysSelect)
    {
        ui->btnSelectObject->setChecked(true);
    }
    m_pGlobalSelector->setSelectState(ui->btnSelectObject->isChecked());
}

void StyleDebugger::showEvent(QShowEvent* event)
{
    if (!m_hasMove)
    {
        //默认右上角
        QRect rect = qApp->desktop()->screenGeometry();
        move(rect.width() - width(), 0);
        m_hasMove = true;
    }

    CHBaseDialog::showEvent(event);
}

void StyleDebugger::initWidget()
{
    //还原自己式样
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

    setTitle(tr("CHStyleDebugger"));
    setFixedHeight(80);

    m_pQssEditor = new CQssEditor(Q_NULLPTR);
    connect(m_pQssEditor, &CQssEditor::visibleChanged, ui->btnStyleSheet, &QPushButton::setChecked);
    m_pPropertyPanel = new CPropertyPanel(Q_NULLPTR);
    connect(m_pPropertyPanel, &CPropertyPanel::visibleChanged, ui->btnProperty, &QPushButton::setChecked);
    m_pGlobalSelector = new CGlobalSelector(this);
    m_pGlobalSelector->setNotHandleEventList(QList<QWidget*>() << this << m_pPropertyPanel << m_pQssEditor);
    connect(m_pGlobalSelector, &CGlobalSelector::selectedWidget, m_pPropertyPanel, [this](QWidget* w)
    {
        m_pPropertyPanel->setWidget(w);
        m_pGlobalSelector->setSelectState(false);
        ui->btnProperty->setChecked(true);
    });
    connect(m_pGlobalSelector, &CGlobalSelector::selectStateChanged, ui->btnSelectObject, &QPushButton::setChecked);

    connect(ui->btnStyleSheet, &QPushButton::clicked, this, &StyleDebugger::toggleQssEditorVisible);
    connect(ui->btnProperty, &QPushButton::clicked, this, &StyleDebugger::togglePropertyPanelVisible);
    connect(ui->btnSelectObject, &QPushButton::clicked, this, &StyleDebugger::selectObject);

    //设置
    auto refreshSettings = [this]()
    {
        ST_CONFIG config = appContext->settings();
        setWindowOpacity(config.mainOpacity);

        m_pQssEditor->setWindowFlag(Qt::WindowStaysOnTopHint, config.qssEditorTop);
        m_pPropertyPanel->setWindowFlag(Qt::WindowStaysOnTopHint, config.propertyTop);
        setWindowFlag(Qt::WindowStaysOnTopHint, config.mainTop);

        this->setVisible(config.showMainWindow);
        if (config.showTrayIcon)
        {
            if (m_trayIcon == Q_NULLPTR)
            {
                // 初始化系统托盘图标
                m_trayIcon = new QSystemTrayIcon(this);
                m_trayIcon->setIcon(QIcon(":/Image/icon_sd.png"));
                m_trayIcon->setContextMenu(ui->btnSettings->menu());
                connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason)
                {
                    if (reason == QSystemTrayIcon::DoubleClick)
                    {
                        this->show();
                        this->activateWindow();
                    }
                });
                connect(qGuiApp, &QGuiApplication::lastWindowClosed, this, [this]()
                {
                    m_trayIcon->hide();
                    m_trayIcon->deleteLater();
                });
            }

            m_trayIcon->show();
            m_trayIcon->showMessage(tr("提示"), tr("Hi~ 式样调试器已启动"), QSystemTrayIcon::Information, 3000);
        }
        else if (m_trayIcon != Q_NULLPTR)
        {
            m_trayIcon->setVisible(config.showTrayIcon);
        }
    };
    connect(appContext, &AppContext::updateSettings, this, refreshSettings);
    refreshSettings();
}

void StyleDebugger::initSettings()
{
    QMenu* menu = new QMenu(this);
    QAction* softwareSetting = new QAction(tr("软件设置"), menu);
    connect(softwareSetting, &QAction::triggered, this, [this]()
    {
        SoftwareSettings dlg(this);
        dlg.exec();
    });
    menu->addAction(softwareSetting);

    QAction* qssSetting = new QAction(tr("美化面板设置"), menu);
    connect(qssSetting, &QAction::triggered, this, [this]()
    {
        QssEditorSettings dlg(this);
        dlg.exec();
    });
    menu->addAction(qssSetting);

    QAction* help = new QAction(tr("帮助"), menu);
    connect(help, &QAction::triggered, this, [this]()
    {
        CHMessageToast::information(this, tr("完善中，敬请期待"));
    });
    menu->addAction(help);

    QAction* about = new QAction(tr("关于"), menu);
    connect(about, &QAction::triggered, this, [this]()
    {
        About dlg(this);
        dlg.exec();
    });
    menu->addAction(about);

    ui->btnSettings->setMenu(menu);
}
