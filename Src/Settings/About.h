#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include "Components/CHBaseDialog.h"

namespace Ui {
class About;
}

class About : public CHBaseDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
