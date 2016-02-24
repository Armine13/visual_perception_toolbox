#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QSpinBox>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QGridLayout>

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

    void on_actionSet_Kernel_Size_triggered();

    void on_actionSalt_Pepper_triggered();

    void on_actionMedian_triggered();

    void on_actionGaussian_triggered();

private:
    Ui::MainWindow *ui;
    cv::Mat openImage;
    int kernel_size;
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionClose_triggered();
    void on_actionNew_triggered();
};

#endif // MAINWINDOW_H
