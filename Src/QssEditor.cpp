#include "Resources.h"
#include "QssEditor.h"
#include "qscilexerqss.h"
#include "ui_QssEditor.h"
#include <QFileDialog>
#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QStyleFactory>

#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QTextCodec>
#include <QKeyEvent>
#include <QTextStream>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDesktopServices>

#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include "AppContext.h"
#include "Components/CHMessageToast.h"
#include "Settings/QssEditorSettings.h"
#include "qscintilla/Qt4Qt5/Qsci/qsciscintilla.h"

CQssEditor::CQssEditor(QWidget *parent) :
    QMainWindow(parent),
    m_hasMove(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("美化面板"));
    setWindowModality(Qt::WindowModal);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    //setWindowFlag(Qt::WindowCloseButtonHint, false);

    connect(ui->listWidget, &QListWidget::doubleClicked, this, [this](const QModelIndex& index)
    {
        if (!index.isValid())
        {
            return;
        }
        QString strFile = ui->listWidget->item(index.row())->data(Qt::UserRole).toString();
        //新建Tab页
        openFileInSoftware(strFile);
    });

    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, [this](const QPoint &pos)
    {
        QListWidgetItem* curItem = ui->listWidget->itemAt(pos);
        if (curItem == Q_NULLPTR)
        {
            return;
        }

        QString strFile = curItem->data(Qt::UserRole).toString();
        QFile fi(strFile);
        if (!fi.exists())
        {
            return;
        }

        QMenu* pMenu = new QMenu(this);
        QAction* actionOpen = new QAction("本地默认软件打开", pMenu);
        connect(pMenu, &QMenu::triggered, this, [this, strFile, actionOpen](QAction* action)
        {
            if (action != actionOpen)
            {
                return;
            }
            QDesktopServices::openUrl(QUrl(strFile));
        });

        pMenu->addAction(actionOpen);
        pMenu->move(QCursor::pos());
        pMenu->exec();

    });

    //搜索相关
    ui->wSearch->hide();
    connect(ui->ledSearch, &QLineEdit::returnPressed, this, &CQssEditor::search);
    connect(ui->btnSearchNext, &QPushButton::clicked, this, &CQssEditor::search);
    // TODO: 暂不支持
    ui->btnSearchPre->hide();
    connect(ui->btnSearchPre, &QPushButton::clicked, this, [this]()
    {
        QString strKey = ui->ledSearch->text();
        if (strKey.isEmpty())
        {
            return;
        }
        QsciScintilla* text = currentText();
        if (text != Q_NULLPTR)
        {
            text->findFirst(strKey, true, false, false, true, false);
        }
    });

    init();

    m_autoRfresh = new QTimer(this);
    m_autoRfresh->setInterval(250);
    connect(m_autoRfresh, &QTimer::timeout, this, [this]()
    {
        refreshStyle();
    });
    connect(ui->btnManualRefresh, &QPushButton::clicked, this, &CQssEditor::manualRefresh);

    //设置监听
    auto refreshSettings = [this]()
    {
        ST_CONFIG config = appContext->settings();
        setWindowOpacity(config.qssEditorOpacity);
    };
    connect(appContext, &AppContext::updateSettings, this, refreshSettings);
    refreshSettings();

    //Qss配置监听
    auto refreshQssConfig = [this]()
    {
        ST_QSS_CONFIG config = appContext->qssConfig();

        initFiles();
        if (config.autoRefresh)
        {
           m_autoRfresh->start();
        }
        else
        {
            m_autoRfresh->stop();
        }
        ui->btnManualRefresh->setVisible(!config.autoRefresh);
    };
    connect(appContext, &AppContext::updateQssConfig, this, refreshQssConfig);
    refreshQssConfig();

    ui->lsvErrorInfo->hide();
}

CQssEditor::~CQssEditor()
{
    delete ui;
}

void CQssEditor::init()
{
    //还原自己
    QString strSelfQss = appContext->getSelfQssInfo();
    this->setStyleSheet(strSelfQss);
    this->style()->unpolish(this);
    this->style()->polish(this);

    connect(ui->btnSettings, &QPushButton::clicked, this, [this]()
    {
        QssEditorSettings settings(this);
        settings.exec();
    });

    qApp->installEventFilter(this);

    //关闭标签保存
    connect(ui->tbwQssFiles, &QTabWidget::tabCloseRequested, this, [this](int index)
    {
        QWidget* w = ui->tbwQssFiles->widget(index);
        QsciScintilla* text = qobject_cast<QsciScintilla*>(w);
        if (text != Q_NULLPTR)
        {
            // SAVE
            QFile fi(text->property("fileName").toString());
            if (fi.open(QIODevice::WriteOnly))
            {
                QTextStream out(&fi);
                // utf-8 bom
                out.setGenerateByteOrderMark(true);
                out.setCodec("UTF-8");
                out << text->text();
            }

            ui->tbwQssFiles->removeTab(index);
            text->deleteLater();
        }
    });

    ui->lsvErrorInfo->setModel(&m_errorModel);
    ui->lsvErrorInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->lsvErrorInfo, &QListView::doubleClicked, this, [this](const QModelIndex& index)
    {
        if (!index.isValid())
        {
            return;
        }
        QString fileName = m_errorModel.data(index, Qt::UserRole + 1).toString();
        qint64 line = m_errorModel.data(index, Qt::UserRole + 2).toULongLong();
        if (fileName.isEmpty())
        {
            return;
        }

        QsciScintilla* text = openFileInSoftware(fileName);
        text->setFocus();
        if (text != Q_NULLPTR)
        {
            text->setCursorPosition(line - 1, 0);
        }
    });
    connect(appContext, &AppContext::receiveDebugInfo, this, &CQssEditor::dealDebugInfo);

    ui->splitterFileDetail->setSizes(QList<int>() << 160 << 700);
    ui->splitterDetailError->setSizes(QList<int>() << 520 << 100);

}

void CQssEditor::initFiles()
{
    QStringList lsFiles = appContext->qssConfig().qssFiles;
    ui->listWidget->clear();
    m_qssFileInfos.clear();
    for (int i = 0; i < lsFiles.length(); ++i)
    {
        QString strFile = lsFiles.at(i);
        ui->listWidget->addItem(QFileInfo(strFile).fileName());
        ui->listWidget->item(ui->listWidget->count() - 1)->setData(Qt::UserRole, strFile);
        ST_QSS_FILE_INFO qssFileInfo;
        qssFileInfo.filePathName = strFile;
        m_qssFileInfos.append(qssFileInfo);
    }
}

QsciScintilla* CQssEditor::currentText()
{
    QsciScintilla* text = Q_NULLPTR;
    if (ui->tbwQssFiles->currentWidget() != Q_NULLPTR)
    {
        text = qobject_cast<QsciScintilla*>(ui->tbwQssFiles->currentWidget());
        return text;
    }
    return Q_NULLPTR;
}

//转换自适应式样表
QString CQssEditor::getScaleStyleSheetByStringType1(const QString& strSheet) const
{
    QString strStyleSheet = strSheet;

    if (strStyleSheet.isEmpty())
    {
        return QString();
    }

    //替换变量
    strStyleSheet = strStyleSheet.replace("$appPath", QFileInfo(QApplication::applicationDirPath()).absolutePath());

    //更新界面大小
    float fAppScale = appContext->qssConfig().dp;
    fAppScale = fAppScale <= 0 ? 1.0 : fAppScale;
    int nIndex = strStyleSheet.indexOf("dp(", 0);

    while (nIndex >= 0)
    {
        int nBracket = strStyleSheet.indexOf("(", nIndex);
        int nInvBracket = strStyleSheet.indexOf(")", nBracket);
        QString strAllPx = strStyleSheet.mid(nIndex, nInvBracket - nIndex + 1);
        QString strSizePx = strStyleSheet.mid(nBracket + 1, nInvBracket - nBracket - 1);

        int nSizePx = 0;

        if (!strSizePx.contains('-') && !strSizePx.contains('+'))
        {
            nSizePx = strSizePx.toInt();
        }
        else
        {
            strSizePx = strSizePx.remove(' ');
            //0: 加法 1:减法
            QList<int> lsnSymbol;

            for (int i = 0; i < strSizePx.length(); i++)
            {
                QChar charOne = strSizePx.at(i);

                if (!charOne.isNumber())
                {
                    if (charOne == '+')
                    {
                        lsnSymbol.append(0);
                    }
                    else if (charOne == '-')
                    {
                        lsnSymbol.append(1);
                    }

                    strSizePx = strSizePx.replace(i, 1, ' ');
                }
            }

            QStringList lsNumbers = strSizePx.split(' ');

            if (lsNumbers.length() == lsnSymbol.length() + 1)
            {
                nSizePx = lsNumbers.at(0).toInt();

                for (int i = 0; i < lsnSymbol.length(); i++)
                {
                    if (lsnSymbol.at(i) == 0)
                    {
                        nSizePx += lsNumbers.at(i + 1).toInt();
                    }
                    else if (lsnSymbol.at(i) == 1)
                    {
                        nSizePx -= lsNumbers.at(i + 1).toInt();
                    }
                }
            }
        }

        int nSizeDp = nSizePx * fAppScale;
        nSizeDp = (nSizeDp <= 0) ? nSizePx : nSizeDp;
        QString strResult = QString::number(nSizeDp) + QString("px");
        strStyleSheet.replace(strAllPx, strResult);
        nIndex = strStyleSheet.indexOf("dp(", nIndex);
    }

    //更新字体大小
    float fFontScale = appContext->qssConfig().sp;
    fFontScale = fFontScale <= 0 ? 1.0 : fFontScale;
    nIndex = strStyleSheet.indexOf("sp(", 0);

    while (nIndex >= 0)
    {
        int nBracket = strStyleSheet.indexOf("(", nIndex);
        int nInvBracket = strStyleSheet.indexOf(")", nBracket);
        QString strAllPx = strStyleSheet.mid(nIndex, nInvBracket - nIndex + 1);
        int nSizePx = strStyleSheet.mid(nBracket + 1, nInvBracket - nBracket - 1).toInt();
        int nSizeSp = nSizePx * fFontScale;
        nSizeSp = (nSizeSp <= 0) ? nSizePx : nSizeSp;
        QString strResult = QString::number(nSizeSp) + QString("px");
        strStyleSheet.replace(strAllPx, strResult);
        nIndex = strStyleSheet.indexOf("sp(", nIndex);
    }

    return strStyleSheet;
}


QString CQssEditor::getScaleStyleSheetByStringType2(const QString& strStyle) const
{
    QString strStyleSheet = strStyle;
    int nIndex = strStyleSheet.indexOf("{", 0);

    //替换变量
    strStyleSheet = strStyleSheet.replace("$appPath", QFileInfo(QApplication::applicationDirPath()).absolutePath());

    while (nIndex >= 0)
    {
        //取花括号的内容
        int nBracket = strStyleSheet.indexOf("{", nIndex);
        int nInvBracket = strStyleSheet.indexOf("}", nBracket);
        QString strItem = strStyleSheet.mid(nIndex, nInvBracket - nIndex + 1);
        //花括号内容修改
        QString strScaleItem = modScaleStyle(strItem);
        //替换花括号的内容
        strStyleSheet.replace(strItem, strScaleItem);
        nIndex = strStyleSheet.indexOf("{", nIndex + strScaleItem.length());
    }

    return strStyleSheet;
}

QString CQssEditor::modScaleStyle(const QString &strItem) const
{
    QString strScaleItem = strItem;
    float fAppScale = 1.0;
    float fFontScale = 1.0;

    int nIndex = strItem.indexOf(":", 0);

    while (nIndex >= 0)
    {
        //按行取--值为冒号和分号之间
        int nBracket = strItem.indexOf(":", nIndex);
        int nInvBracket = strItem.indexOf(";", nBracket);
        QString strLine = strItem.mid(nIndex, nInvBracket - nIndex + 1);
        QString strSrcLine = strLine;

        //dp计算
        while (strLine.indexOf("dp ") > 0 || strLine.indexOf("dp;") > 0)
        {
            QString strDp;
            int nStart;
            int nEnd;
            int nValue;
            nEnd = strLine.indexOf("dp");
            strDp = strLine.mid(0, nEnd + 2).trimmed();
            nStart = strDp.lastIndexOf(" ");
            nStart = nStart > 0 ? nStart : 1;
            strDp = strDp.mid(nStart);
            nValue = strDp.mid(0, strDp.length() - 2).toInt() * fAppScale;
            nValue = nValue == 0 ? strDp.mid(0, strDp.length() - 2).toInt() : nValue;
            strLine.replace(strDp.trimmed(), QString::number(nValue) + "px");
        }

        //sp计算
        while (strLine.indexOf("sp ") > 0 || strLine.indexOf("sp;") > 0)
        {
            QString strSp;
            int nStart;
            int nEnd;
            int nValue;
            nEnd = strLine.indexOf("sp");
            strSp = strLine.mid(0, nEnd + 2).trimmed();
            nStart = strSp.lastIndexOf(" ");
            nStart = nStart > 0 ? nStart : 1;
            strSp = strSp.mid(nStart);
            nValue = strSp.mid(0, strSp.length() - 2).toInt() * fFontScale;
            nValue = nValue == 0 ? strSp.mid(0, strSp.length() - 2).toInt() : nValue;
            strLine.replace(strSp.trimmed(), QString::number(nValue) + "px");
        }

        //替换值
        strScaleItem.replace(strSrcLine, strLine);

        //下一行
        nIndex = strItem.indexOf(":", nIndex + strSrcLine.length());
    }

    return strScaleItem;
}

void CQssEditor::refreshStyle(const bool bForceRefresh)
{
    QFile fleQss;
    QString strStyleSheet;

    int nCount = m_qssFileInfos.count();
    bool bHasModify = false;
    for (int i = 0; i < nCount; ++i)
    {
        bool singleModify = false;
        ST_QSS_FILE_INFO qssFileInfo = m_qssFileInfos.at(i);
        QFileInfo fiQss;
        fleQss.setFileName(qssFileInfo.filePathName);
        fiQss.setFile(fleQss);

        if (qssFileInfo.lastModifiedTime != fiQss.lastModified())
        {
            singleModify = true;
            bHasModify = true;
        }

        if (singleModify)
        {
            fleQss.open(QFile::ReadOnly);
            if (fleQss.isOpen())
            {
                QTextStream in(&fleQss);
                QString qssContent = in.readAll();
                if (appContext->qssConfig().scaleType == 0)
                {
                    qssFileInfo.fileContent = qssContent;
                }
                else if (appContext->qssConfig().scaleType == 1)
                {
                    qssFileInfo.fileContent = getScaleStyleSheetByStringType1(qssContent);
                }
                else
                {
                    qssFileInfo.fileContent = getScaleStyleSheetByStringType2(qssContent);
                }

                strStyleSheet.append(qssFileInfo.fileContent);
                fleQss.close();
                QFileInfo fileInfo(fleQss);
                qssFileInfo.lastModifiedTime = fileInfo.lastModified();
                qssFileInfo.errorInfos.clear();
                qssFileInfo.errorLines.clear();
            }
            m_qssFileInfos.replace(i, qssFileInfo);
        }
        else
        {
            strStyleSheet.append(qssFileInfo.fileContent);
        }
    }

    if (bHasModify || bForceRefresh)
    {
        m_unknowProperty.clear();
        appContext->setStyleSheet(strStyleSheet);

        //还原自己
        QString strSelfQss = appContext->getSelfQssInfo();
        this->setStyleSheet(strSelfQss);
        this->style()->unpolish(this);
        this->style()->polish(this);
        Q_EMIT appContext->updateQssInfo();

        checkQssParse();
    }
}

void CQssEditor::checkQssParse()
{
    m_errorModel.clear();
    for (int f = 0; f < m_qssFileInfos.length(); f++)
    {
        ST_QSS_FILE_INFO qssFileInfo = m_qssFileInfos.at(f);
        QString qssTrans = qssFileInfo.fileContent;

        QCss::Parser parser(qssTrans, false);
        QCss::StyleSheet ss;
        bool bRet = parser.parse(&ss);
        if (!bRet)
        {
            int rowIndex = 0;
            if (parser.errorIndex != -1)
            {
                QCss::Symbol sbl;
                if (parser.errorIndex >= parser.symbols.length())
                {
                    sbl = parser.symbols.at(parser.errorIndex - 1);
                }
                else
                {
                    sbl = parser.errorSymbol();
                }

                for (int i = 0; i < sbl.text.length(); i++)
                {
                    if (sbl.text.data()[i] == '\n')
                    {
                        if (i >= sbl.start)
                        {
                            break;
                        }
                        rowIndex++;
                    }
                }
            }

            if (qssFileInfo.errorInfos.contains(tr("解析错误")) &&
                qssFileInfo.errorLines.contains(rowIndex + 1))
            {
                continue;
            }

            qssFileInfo.errorInfos.append(tr("解析错误"));
            qssFileInfo.errorLines.append(rowIndex + 1);
        }
        m_qssFileInfos.replace(f, qssFileInfo);
    }
    showErrorInfo();
}

void CQssEditor::checkProperty(const QString& property)
{
    for (int i = 0; i < m_qssFileInfos.length(); i++)
    {
        bool hasError = false;
        ST_QSS_FILE_INFO qssFileInfo = m_qssFileInfos.at(i);
        QStringList lsLine = qssFileInfo.fileContent.split("\n");
        for (int l = 0; l < lsLine.length(); l++)
        {
            QString line = lsLine.at(l);
            line = line.remove(' ');
            if (line.contains(property + ":"))
            {
                if (qssFileInfo.errorInfos.contains(tr("未知属性 ") + property) &&
                    qssFileInfo.errorLines.contains(l + 1))
                {
                    continue;
                }

                qssFileInfo.errorInfos.append(tr("未知属性 ") + property);
                qssFileInfo.errorLines.append(l + 1);
                hasError = true;
            }
        }
        m_qssFileInfos.replace(i, qssFileInfo);
    }
    showErrorInfo();
}

void CQssEditor::showErrorInfo()
{
    // TODO: 改成列表可跳转
    for (int i = 0; i < m_qssFileInfos.length(); i++)
    {
        ST_QSS_FILE_INFO qssFileInfo = m_qssFileInfos.at(i);
        for (int e = 0; e < qssFileInfo.errorInfos.length(); e++)
        {
            QString error = QString(tr("文件\"%1\"在%2行: %3！\r\n")).arg(QFileInfo(qssFileInfo.filePathName).fileName())
                    .arg(qssFileInfo.errorLines.at(e)).arg(qssFileInfo.errorInfos.at(e));
            QStandardItem* item = new QStandardItem(error);
            item->setData(qssFileInfo.filePathName, Qt::UserRole + 1);
            item->setData(qssFileInfo.errorLines.at(e), Qt::UserRole + 2);
            m_errorModel.appendRow(item);
        }
    }
    if (m_errorModel.rowCount() <= 0)
    {
        ui->lsvErrorInfo->hide();
    }
    else
    {
        ui->lsvErrorInfo->show();
    }
}

void CQssEditor::manualRefresh()
{
    refreshStyle(true);
}

//void CQssEditor::on_chbAutoRefresh_toggled(bool checked)
//{
//    if (checked)
//    {
//        m_autoRfresh->start();
//    }
//    else
//    {
//        m_autoRfresh->stop();
//    }
//}

bool CQssEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (!this->isActiveWindow())
    {
        return QObject::eventFilter(obj, event);
    }

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if(ke)
        {
            QsciScintilla* text = currentText();
            if (text == Q_NULLPTR)
            {
                return false;
            }

            if((ke->modifiers() == Qt::CTRL) && (ke->key() == Qt::Key_Space))
            {
                text->autoCompleteFromAll();
                return true;
            }
            if ((ke->modifiers() == Qt::CTRL) && (ke->key() == Qt::Key_S))
            {
                QFile fi(text->property("fileName").toString());
                if (!fi.open(QIODevice::WriteOnly))
                {
                    return false;
                }

                QTextStream out(&fi);
                // utf-8 bom
                out.setGenerateByteOrderMark(true);
                out.setCodec("UTF-8");
                out << text->text();

                CHMessageToast::success(this, tr("保存成功。"));
                return true;
            }
            if ((ke->modifiers() == Qt::CTRL) && (ke->key() == Qt::Key_F))
            {
                ui->wSearch->setVisible(!ui->wSearch->isVisible());
                if (ui->wSearch->isVisible())
                {
                    QTimer::singleShot(100, this, [this]
                    {
                        ui->ledSearch->activateWindow();
                        ui->ledSearch->setFocus();
                    });
                }
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

void CQssEditor::closeEvent(QCloseEvent *e)
{
    // TODO: 关闭保存
//    if(continueWhenUnsaved())
//    {
//        saveLastFiles();
//        e->accept();
//    }
//    else
    //        e->ignore();
    QWidget::closeEvent(e);
}

void CQssEditor::showEvent(QShowEvent* event)
{
    if (!m_hasMove)
    {
        //默认右上角
        QRect rect = qApp->desktop()->screenGeometry();
        move(rect.width() - width() - 20, 120);
        m_hasMove = true;
    }
    QWidget::showEvent(event);
    Q_EMIT visibleChanged(isVisible());
}

void CQssEditor::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    Q_EMIT visibleChanged(isVisible());
}

//搜索
void CQssEditor::search()
{
    QString strKey = ui->ledSearch->text();
    if (strKey.isEmpty())
    {
        return;
    }

    QsciScintilla* text = currentText();
    if (text != Q_NULLPTR)
    {
        text->findFirst(strKey, true, false, false, true);
    }
}

QsciScintilla* CQssEditor::openFileInSoftware(const QString& strFileName)
{
    QWidget* hasWidget = Q_NULLPTR;
    for (int i = 0; i < ui->tbwQssFiles->count(); i++)
    {
        QWidget* widget = ui->tbwQssFiles->widget(i);
        QString strName = widget->property("fileName").toString();
        if (strName == strFileName)
        {
            hasWidget = widget;
            break;
        }
    }
    if (hasWidget != Q_NULLPTR)
    {
        ui->tbwQssFiles->setCurrentWidget(hasWidget);
        return qobject_cast<QsciScintilla*>(hasWidget);
    }

    //Qss编辑器相关
    QsciScintilla* text = new QsciScintilla(ui->tbwQssFiles);
    text->setProperty("fileName", strFileName);
    text->setCaretLineVisible(true);
    text->setCaretLineBackgroundColor(QColor("gainsboro"));

    text->setAutoIndent(true);
    text->setIndentationGuides(false);
    text->setIndentationsUseTabs(true);
    text->setIndentationWidth(4);

    text->setMarginsBackgroundColor(QColor("gainsboro"));
    text->setMarginLineNumbers(1, true);
    text->setMarginWidth(1, 50);

    text->setAutoCompletionSource(QsciScintilla::AcsAll);
    text->setAutoCompletionReplaceWord(true);
    text->setAutoCompletionCaseSensitivity(false);
    text->setAutoCompletionUseSingle(QsciScintilla::AcusExplicit);
    text->setAutoCompletionThreshold(1);

    text->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    text->setMatchedBraceBackgroundColor(Qt::yellow);
    text->setUnmatchedBraceForegroundColor(Qt::blue);

    text->setUtf8(true);
    // QSS lexer
    QsciLexerQSS *lexQss = new QsciLexerQSS(text);
    text->setLexer(lexQss);

    text->setFocus();

    ui->tbwQssFiles->addTab(text, QFileInfo(strFileName).fileName());
    ui->tbwQssFiles->setCurrentWidget(text);

    if (text->text().isEmpty())
    {
        //打开文件
        QFile fi(strFileName);
        if (!fi.exists())
        {
            return Q_NULLPTR;
        }

        if (!fi.open(QIODevice::ReadOnly))
        {
            return Q_NULLPTR;
        }

        QTextStream in(&fi);
        QTextCodec* code = QTextCodec::codecForName("UTF-8");
        in.setCodec(code);
        QString qss = in.readAll();
        text->setText(qss);
    }

    return text;
}

//收到调试信息
void CQssEditor::dealDebugInfo(const QString& file, const QString& context)
{
    Q_UNUSED(file);

    // qss解析有问题
    if (context.contains("Could not parse application stylesheet"))
    {
        // NOTE: 保存的时候已校验
        //checkQssParse();;
    }

    //属性设置有问题
    if (context.contains("Unknown property "))
    {
        QString propertyName = context;
        propertyName = propertyName.remove("Unknown property ");

        if (!m_unknowProperty.contains(context))
        {
            checkProperty(propertyName);
            m_unknowProperty.append(context);
        }
    }
}
