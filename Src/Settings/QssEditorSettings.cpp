#include <QFileInfo>
#include <QFileDialog>
#include "QssEditorSettings.h"
#include "ui_QssEditorSettings.h"
#include "AppContext.h"
#include "Resources.h"
#include "Components/CHMessageToast.h"

QssEditorSettings::QssEditorSettings(QWidget *parent) :
    CHBaseDialog(parent),
    ui(new Ui::QssEditorSettings)
{
    ui->setupUi(getFunctionWidget());
    setWindowFlag(Qt::Dialog);

    initWidget();
}

QssEditorSettings::~QssEditorSettings()
{
    delete ui;
}

void QssEditorSettings::showEvent(QShowEvent* event)
{
    CHBaseDialog::showEvent(event);
    if (ui->lswQssFiles->count() <= 0)
    {
        CHMessageToast::warning(this, tr("请先添加qss文件"));
    }
}

void QssEditorSettings::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
                              this,
                              "选择qss文件",
                              "",
                              "式样文件 (*.qss *.css *.text)");
    for (int i = 0; i < files.length(); ++i)
    {
        QString strFile = files.at(i);
        if (m_qssFiles.indexOf(strFile) >= 0)
        {
            continue;
        }
        m_qssFiles.append(strFile);
        ui->lswQssFiles->addItem(QFileInfo(strFile).fileName());
        ui->lswQssFiles->item(ui->lswQssFiles->count() - 1)->setData(Qt::UserRole, strFile);
    }
}

void QssEditorSettings::removeFile()
{
    int nIndex = ui->lswQssFiles->currentRow();
    if (nIndex < 0)
    {
        return;
    }
    bool bRet = ui->lswQssFiles->model()->removeRow(nIndex);
    if (bRet)
    {
        m_qssFiles.removeAt(nIndex);
    }
}

void QssEditorSettings::upFile()
{
    int nIndex = ui->lswQssFiles->currentRow();
    if (nIndex <= 0)
    {
        return;
    }

    m_qssFiles.move(nIndex, nIndex - 1);
    resetFileList();
    ui->lswQssFiles->setCurrentRow(nIndex - 1);
}

void QssEditorSettings::downFile()
{
    int nIndex = ui->lswQssFiles->currentRow();
    if (nIndex < 0 || nIndex>= ui->lswQssFiles->count() - 1)
    {
        return;
    }

    m_qssFiles.move(nIndex, nIndex + 1);
    resetFileList();
    ui->lswQssFiles->setCurrentRow(nIndex + 1);
}

void QssEditorSettings::saveConfig()
{
    ST_QSS_CONFIG config = appContext->qssConfig();
    config.qssFiles = m_qssFiles;
    config.scaleType = ui->rbtnAuto->isChecked() ? 0 : ui->rbtnType1->isChecked() ? 1 : 2;
    config.sp = ui->ledSp->text().trimmed().toDouble();
    config.dp = ui->ledDp->text().trimmed().toDouble();
    config.autoRefresh = ui->chbAutoRefresh->isChecked();

    appContext->setQssConfig(config);

    accept();
}

void QssEditorSettings::initWidget()
{
    setTitle(tr("美化面板设置"));
    setFixedSize(700, 450);

    ST_QSS_CONFIG config = appContext->qssConfig();
    ui->ledDp->setText(QString::number(config.dp));
    ui->ledSp->setText(QString::number(config.sp));
    if (config.scaleType == 0)
    {
        ui->rbtnAuto->setChecked(true);
    }
    else if (config.scaleType == 1)
    {
        ui->rbtnType1->setChecked(true);
    }
    else
    {
        ui->rbtnType2->setChecked(true);
    }
    ui->chbAutoRefresh->setChecked(config.autoRefresh);
    m_qssFiles = config.qssFiles;
    resetFileList();

    ui->lswQssFiles->setDragEnabled(true);
    ui->lswQssFiles->setDragDropMode(QAbstractItemView::InternalMove);
    connect(ui->lswQssFiles->model(), &QAbstractItemModel::rowsMoved, this, [this]()
    {
        m_qssFiles.clear();
        for (int i = 0; i < ui->lswQssFiles->count(); i++)
        {
            m_qssFiles.append(ui->lswQssFiles->item(i)->data(Qt::UserRole).toString());
        }
    });

    connect(ui->btnAddFile, &QPushButton::clicked, this, &QssEditorSettings::addFiles);
    connect(ui->btnDeleteFile, &QPushButton::clicked, this, &QssEditorSettings::removeFile);
    connect(ui->btnUpFile, &QPushButton::clicked, this, &QssEditorSettings::upFile);
    connect(ui->btnDownFile, &QPushButton::clicked, this, &QssEditorSettings::downFile);

    connect(ui->btnSave, &QPushButton::clicked, this, &QssEditorSettings::saveConfig);
}

void QssEditorSettings::resetFileList()
{
    ui->lswQssFiles->clear();
    for (int i = 0; i < m_qssFiles.length(); ++i)
    {
        QString strFile = m_qssFiles.at(i);
        ui->lswQssFiles->addItem(QFileInfo(strFile).fileName());
        ui->lswQssFiles->item(ui->lswQssFiles->count() - 1)->setData(Qt::UserRole, strFile);
    }
}
