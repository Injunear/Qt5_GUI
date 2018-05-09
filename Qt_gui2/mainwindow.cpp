#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mThread = new mythread(this);
    mThread->start(QThread::HighestPriority); //

    QObject::connect(mThread, &mythread::Number1Changed, this, &MainWindow::onNumber1Changed); // Qt5
    // Doesn't like (int) after "Changed"

  //  QObject::connect(this, &MainWindow::doStopRunning, mThread, &mythread::stopRunning); // Qt5

    connect(mThread, SIGNAL(Number2Changed(int)), this, SLOT(onNumber2Changed(int))); // <Qt5

 //   connect(this, SIGNAL(doStopRunning()), mThread, SLOT(mThread::stopRunning()));
  //  connect(this, SIGNAL(doStopRunning()), mThread, SLOT(quit()));
  //  connect(this, SIGNAL(doStopRunning()), mThread, SLOT(terminate()));

}


MainWindow::~MainWindow()
{
    //qDebug() << "MainWindow delete routine";
    int a=0, b=0, fd1=0, fd2=0;
    // Set outputs to zero
    fd1=wiringPiI2CSetup(32);
    fd2=wiringPiI2CSetup(33);
    a = wiringPiI2CWriteReg8(fd1,18,129);
    b = wiringPiI2CWriteReg8(fd2,18,129);
   // for(int a=0; a<10000; ++a);
    emit MainWindow::doStopRunning();
    for(int a=0; a<10000; ++a);
    //  delete mThread;
    delete ui;

    qDebug() << "MainWindow deleted";
}


void MainWindow::doStopRunning() // Signal
{
    qDebug() << "doStopRunning signal";
    //mThread->running = false;
    //mThread->quit();
}


void MainWindow::on_ToggleButton1_clicked() // Signal
{
    // Write to 0x20
    qDebug() << "Button 1";
}

void MainWindow::on_ToggleButton2_clicked() // Signal
{
    // Write to 0x21
    qDebug() << "Button 2";

}

void MainWindow::onNumber1Changed(int x) // slot
{
    ui->lineEdit1->setText(QString::number(x));
}

void MainWindow::onNumber2Changed(int x) // slot
{
    ui->lineEdit2->setText(QString::number(x));
}
