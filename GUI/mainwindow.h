#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionMean_triggered();

private:
    Ui::MainWindow *ui;
    cv::Mat openImage;
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionClose_triggered();

    void on_actionNew_triggered();
};

#endif // MAINWINDOW_H
