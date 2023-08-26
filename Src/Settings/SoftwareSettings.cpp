#include <QStyle>
#include <QColorDialog>
#include "SoftwareSettings.h"
#include "ui_SoftwareSettings.h"
#include "Resources.h"
#include "AppContext.h"

SoftwareSettings::SoftwareSettings(QWidget *parent) :
    CHBaseDialog(parent),
    ui(new Ui::SoftwareSettings)
{
    ui->setupUi(getFunctionWidget());
    setWindowFlag(Qt::Dialog);

    initWidget();
}

SoftwareSettings::~SoftwareSettings()
{
    delete ui;
}

void SoftwareSettings::saveSettings()
{
    ST_CONFIG config = appContext->settings();
    config.mainTop = ui->chbMainTop->isChecked();
    config.propertyTop = ui->chbPropertyTop->isChecked();
    config.qssEditorTop = ui->chbQssEditorTop->isChecked();

    config.mainOpacity = (double)ui->hsidMainOpacity->value() / 100;
    config.propertyOpacity = (double)ui->hsidPropertyOpacity->value() / 100;
    config.qssEditorOpacity = (double)ui->hsidQssEditorOpacity->value() / 100;

    config.mainShortcut = ui->ledMainShortcut->text().trimmed();
    config.propertyShortcut = ui->ledPropertyShortcut->text().trimmed();
    config.qssEditorShortcut = ui->ledQssEditorShortcut->text().trimmed();

    config.objectBorderColor = QColor(ui->ledObjectBorderColor->text().trimmed());
    config.objectBorderWidth = ui->spbObjectBorderWidth->value();
    config.objectKeyShortcut = ui->ledObjectKeyShortcut->text().trimmed();

    config.showTrayIcon = ui->cbTrayIcon->isChecked();
    config.showMainWindow = ui->cbMainWindow->isChecked();

    appContext->setSettings(config);

    accept();
}

void SoftwareSettings::setLineWidth(int value)
{
    ui->lblObjectLine->setMaximumHeight(value);
}

void SoftwareSettings::initWidget()
{
    setTitle(tr("软件设置"));
    setFixedSize(850, 450);

    //初始化设置
    ST_CONFIG config = appContext->settings();
    ui->chbMainTop->setChecked(config.mainTop);
    ui->chbPropertyTop->setChecked(config.propertyTop);
    ui->chbQssEditorTop->setChecked(config.qssEditorTop);

    ui->hsidMainOpacity->setValue(config.mainOpacity * 100);
    ui->hsidPropertyOpacity->setValue(config.propertyOpacity * 100);
    ui->hsidQssEditorOpacity->setValue(config.qssEditorOpacity * 100);

    ui->ledMainShortcut->setText(config.mainShortcut);
    ui->ledPropertyShortcut->setText(config.propertyShortcut);
    ui->ledQssEditorShortcut->setText(config.qssEditorShortcut);

    ui->ledObjectBorderColor->setText(config.objectBorderColor.name());
    ui->spbObjectBorderWidth->setValue(config.objectBorderWidth);
    ui->ledObjectKeyShortcut->setText(config.objectKeyShortcut);

    ui->cbTrayIcon->setChecked(config.showTrayIcon);
    ui->cbMainWindow->setChecked(config.showMainWindow);

    //初始化对象颜色
    auto refeshLine = [this](QColor color)
    {
        ui->btnObjectColor->setStyleSheet(QString("QPushButton#btnObjectColor{background-color:%1; "
                                                  "border-radius: 0px; border: 0px; max-width:20px; "
                                                  "width: 20px; min-width: 20px; padding: 0px; margin: 0px;"
                                                  "height: 20px; min-height: 20px; max-height: 20px;}").arg(color.name()));
        ui->lblObjectLine->setStyleSheet(QString("QLabel{background-color:%1;}").arg(color.name()));
        ui->btnObjectColor->style()->unpolish(ui->btnObjectColor);
        ui->btnObjectColor->style()->polish(ui->btnObjectColor);
        ui->btnObjectColor->setProperty("lineColor", color.name());
        ui->lblObjectLine->style()->unpolish(ui->lblObjectLine);
        ui->lblObjectLine->style()->polish(ui->lblObjectLine);
    };
    refeshLine(config.objectBorderColor);
    connect(ui->btnObjectColor, &QPushButton::clicked, this, [this, refeshLine]()
    {
        QColor color = QColorDialog::getColor(QColor(ui->btnObjectColor->property("lineColor").toString()), this, tr("请选择颜色"));
        if (!color.isValid())
        {
            return;
        }

        ui->ledObjectBorderColor->setText(color.name());
        refeshLine(color);
    });

    ui->lblObjectLine->setMaximumHeight(config.objectBorderWidth);
    connect(ui->ledObjectBorderColor, &QLineEdit::textChanged, this, [refeshLine](const QString& strColor)
    {
        QColor color = QColor(strColor);
        if (!color.isValid())
        {
            return;
        }
        refeshLine(color);
    });
    connect(ui->spbObjectBorderWidth, SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));

    connect(ui->btnSave, &QPushButton::clicked, this, &SoftwareSettings::saveSettings);
}
