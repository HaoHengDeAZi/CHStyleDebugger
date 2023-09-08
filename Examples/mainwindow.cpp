#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include <QDir>
#include <QFileSystemModel>

/*执行字符集*/
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Absoulte Button Test
    QPushButton* p = new QPushButton(this);
    p->move(200, 300);
    p->setObjectName("btnHeiHeiHei");
    p->setText(tr("绝对布局按钮"));
    p->show();

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    ui->treeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::warning(this, "warring", "Just a Warring Box !");
}
