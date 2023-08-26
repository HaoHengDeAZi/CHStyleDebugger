#include "About.h"
#include "ui_About.h"
#include "Resources.h"
#include <QDebug>

#define D_VERSION(V)      #V
#define STR_MACRO(V)      D_VERSION(V)

About::About(QWidget *parent) :
    CHBaseDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(getFunctionWidget());
    setWindowFlag(Qt::Dialog);
    setTitle(tr("关于"));

    ui->lblVersion->setText(QString(tr("版本：V%1")).arg(STR_MACRO(DEBUGGER_VERSION)));
}

About::~About()
{
    delete ui;
}
