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
#include <QLabel>
#include <QVector>
#include <QMessageBox>
#include "window.h"
#include "ui_mainwindow.h"
#include "imageprocessing.h"
#include "player.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual void closeEvent ( QCloseEvent * event);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionMean_triggered();

    void on_actionSet_Kernel_Size_triggered();

    void on_actionMedian_triggered();

    void on_actionGaussian_triggered();

    void on_actionSalt_and_Pepper_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_focusChanged();

    void on_actionOpen_Video_triggered();

    void on_stopVideo();
    void updatePlayerUI(QImage img);

private:
    QVector<Window*> windowList;
    Ui::MainWindow *ui;
    int kernel_size;
    Window* activeWindow;
    Player* vPlayer;
};

#endif // MAINWINDOW_H
