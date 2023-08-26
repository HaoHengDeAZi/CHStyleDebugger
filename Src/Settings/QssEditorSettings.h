#ifndef QSSEDITORSETTINGS_H
#define QSSEDITORSETTINGS_H

#include <QWidget>
#include "Components/CHBaseDialog.h"

namespace Ui {
class QssEditorSettings;
}

class QssEditorSettings : public CHBaseDialog
{
    Q_OBJECT

public:
    explicit QssEditorSettings(QWidget *parent = 0);
    ~QssEditorSettings();

protected:
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void addFiles();
    void removeFile();
    void upFile();
    void downFile();

    void saveConfig();

private:
    void initWidget();
    void resetFileList();

private:
    QStringList m_qssFiles;
    Ui::QssEditorSettings *ui;
};

#endif // QSSEDITORSETTINGS_H
