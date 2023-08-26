/**
  * qss编辑器
  * @author   	ch
  *
*/

#ifndef CQSSEDITOR_H
#define CQSSEDITOR_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QStringList>
#include <QStandardItemModel>
#include <QtGui/private/qcssparser_p.h>
//#include <QtWidgets/private/qstylesheetstyle_p.h>

namespace Ui {
class MainWindow;
}
class QsciScintilla;

class CQssEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit CQssEditor(QWidget *parent = 0);
    ~CQssEditor();

    typedef struct stQssFileInfo
    {
        QString filePathName;
        QString fileContent;
        QList<qint64> errorLines;
        QStringList errorInfos;
        QDateTime lastModifiedTime;

        stQssFileInfo()
        {

        }
    } ST_QSS_FILE_INFO;

Q_SIGNALS:
    void updateQss(const QString& strSelfQss);

    void visibleChanged(const bool visible);
protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void manualRefresh();
    //搜索
    void search();
    //打开文件
    QsciScintilla* openFileInSoftware(const QString& strFileName);
    //收到调试信息
    void dealDebugInfo(const QString& file, const QString& context);

private:
    void init();
    void initFiles();
    QsciScintilla* currentText();

    //转换自适应式样表
    QString getScaleStyleSheetByStringType1(const QString& strSheet) const;
    QString getScaleStyleSheetByStringType2(const QString& strSheet) const;
    //dp、sp值计算
    QString modScaleStyle(const QString& strItem) const;
    void refreshStyle(const bool bForceRefresh = false);

    void checkQssParse();
    void checkProperty(const QString& property);
    void showErrorInfo();

private:
    bool m_hasMove;
    Ui::MainWindow *ui;
    QTimer* m_autoRfresh;

    QStringList m_unknowProperty;
    QStandardItemModel m_errorModel;
    QList<ST_QSS_FILE_INFO> m_qssFileInfos;
};

#endif // CQSSEDITOR_H
