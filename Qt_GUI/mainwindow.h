#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mythread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    mythread *mThread;




//signals:
    void doStopRunning();
    void on_ToggleButton1_clicked();
    void on_ToggleButton2_clicked();


public slots:
    void onNumber1Changed(int);
    void onNumber2Changed(int);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
