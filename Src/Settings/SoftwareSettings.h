#ifndef SOFTWARESETTINGS_H
#define SOFTWARESETTINGS_H

#include <QWidget>
#include "Components/CHBaseDialog.h"

namespace Ui {
class SoftwareSettings;
}

class SoftwareSettings : public CHBaseDialog
{
    Q_OBJECT

public:
    explicit SoftwareSettings(QWidget *parent = 0);
    ~SoftwareSettings();

private Q_SLOTS:
    void saveSettings();
    void setLineWidth(int value);

private:
    void initWidget();

private:
    Ui::SoftwareSettings *ui;
};

#endif // SOFTWARESETTINGS_H
