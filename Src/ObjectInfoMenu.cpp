#include "Resources.h"
#include "ObjectInfoMenu.h"
#include "ui_ObjectInfoMenu.h"
#include <QListWidgetItem>
#include <QLabel>
#include <Components/CHPushButton.h>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include "Components/CHMessageToast.h"
#include <QDebug>
#include <QMenu>
#include <QFontMetrics>
#include <QScreen>

ObjectInfoMenu::ObjectInfoMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectInfoMenu),
    m_currentWidget(Q_NULLPTR),
    m_selectWidget(Q_NULLPTR)
{
    ui->setupUi(this);

    initWidget();
}

ObjectInfoMenu::~ObjectInfoMenu()
{
    delete ui;
}

void ObjectInfoMenu::addWidget(QWidget* widget)
{
    if (widget == Q_NULLPTR)
    {
        return;
    }

    QString strClassName = QString::fromLatin1(widget->metaObject()->className());
    QString strObjectName = widget->objectName();
    if (strObjectName.isEmpty())
    {
        strObjectName = tr("未定义");
    }
    QListWidgetItem* item = new QListWidgetItem();
    ui->lswClassInfo->addItem(item);

    CHPushButton* itemWidget = new CHPushButton(ui->lswClassInfo);
    itemWidget->setObjectName("listWidgetBgButton");
    connect(itemWidget, &CHPushButton::hovered, this, [this](bool hovered)
    {
        Q_UNUSED(hovered);
        CHPushButton* item = qobject_cast<CHPushButton*>(sender());
        if (m_currentWidget == m_itemWidget.value(item))
        {
            return;
        }

        m_currentWidget = m_itemWidget.value(item);
        Q_EMIT hoveredWidget(m_currentWidget);
    });

    connect(itemWidget, &CHPushButton::clicked, this, [this]()
    {
        CHPushButton* item = qobject_cast<CHPushButton*>(sender());
        if (item == Q_NULLPTR || !m_itemWidget.contains(item))
        {
            return;
        }

        m_selectWidget = m_itemWidget.value(item);
        Q_EMIT selectedWidget(m_selectWidget);

        eventLoop.exit();
    });

    itemWidget->setMouseTracking(false);
    itemWidget->setLayout(new QHBoxLayout);
    itemWidget->layout()->setContentsMargins(0, 0, 0, 0);
    QLabel* objectName = new QLabel(strObjectName, itemWidget);
    itemWidget->layout()->addWidget(objectName);
    QFontMetrics fontMatrics(objectName->font());
    objectName->setText(fontMatrics.elidedText(strObjectName, Qt::ElideMiddle, 220));
    QPushButton* copyObject = new QPushButton(itemWidget);
    copyObject->setObjectName("copyClassInfo");
    copyObject->setMaximumWidth(24);
    itemWidget->layout()->addWidget(copyObject);
    connect(copyObject, &QPushButton::clicked, this, [this, strObjectName]()
    {
        qApp->clipboard()->setText(strObjectName);
        CHMessageToast::success(this, tr("复制成功"));
    });

    QLabel* className = new QLabel(strClassName, itemWidget);
    className->setText(fontMatrics.elidedText(strClassName, Qt::ElideMiddle, className->width()));
    itemWidget->layout()->addWidget(className);
    QPushButton* copyClass = new QPushButton(itemWidget);
    copyClass->setObjectName("copyClassInfo");
    connect(copyClass, &QPushButton::clicked, this, [this, strClassName]()
    {
        qApp->clipboard()->setText(strClassName);
        CHMessageToast::success(this, tr("复制成功"));
    });
    copyClass->setMaximumWidth(24);
    itemWidget->layout()->addWidget(copyClass);

    QPushButton* menuButton = new QPushButton(itemWidget);
    menuButton->setObjectName("menuButton");
    connect(menuButton, &QPushButton::clicked, this, [this, strObjectName, strClassName, menuButton]()
    {
        //自定义快捷复制
        QMenu* menu = new QMenu(qobject_cast<QWidget*>(sender()));
        QString classObject = QString("%1#%2").arg(strClassName).arg(strObjectName);
        menu->addAction(classObject);
        menu->addAction(classObject + "\r\n{\r\n}");
        connect(menu, &QMenu::triggered, this, [this](QAction* action)
        {
            qApp->clipboard()->setText(action->text());
            CHMessageToast::success(this, tr("复制成功"));
        });
        menu->exec(menuButton->mapToGlobal(QPoint(menuButton->width(), menuButton->height())));
        menu->deleteLater();
    });
    itemWidget->layout()->addWidget(menuButton);

    ui->lswClassInfo->setItemWidget(item, itemWidget);
    m_itemWidget.insert(itemWidget, widget);
    QSize sizeHint = item->sizeHint();
    sizeHint.setHeight(itemWidget->height());
    item->setSizeHint(sizeHint);

    //调整大小
    QRect rect = geometry();
    rect.setHeight(ui->lblClassName->height() + ui->lswClassInfo->count() * 26 + 40);
    setGeometry(rect);
}

void ObjectInfoMenu::clear()
{
    m_selectWidget = Q_NULLPTR;
    m_currentWidget = Q_NULLPTR;
    m_itemWidget.clear();
    ui->lswClassInfo->clear();
}

QWidget* ObjectInfoMenu::exec(const QPoint& p)
{
    QPoint adjust = p;
    QSize s = qApp->primaryScreen()->availableVirtualSize();
    if (p.x() + width() > s.width())
    {
        adjust.setX(p.x() - width());
    }

    if (p.y() + height() > s.height())
    {
        adjust.setY(p.y() - height());
    }

    this->move(adjust);
    this->show();
    eventLoop.exec();
    this->hide();
    return m_selectWidget;
}

int ObjectInfoMenu::count()
{
    return ui->lswClassInfo->count();
}

void ObjectInfoMenu::initWidget()
{
    setWindowFlag(Qt::Popup);
    setFixedWidth(540);
}

void ObjectInfoMenu::hideEvent(QHideEvent* hideEvent)
{
    eventLoop.exit();
    QWidget::hideEvent(hideEvent);
}
