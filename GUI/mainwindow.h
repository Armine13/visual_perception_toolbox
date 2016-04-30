#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include <cv.h>
//#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include<opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/stitching/stitcher.hpp>

#include <stdio.h>
#include <iostream>

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
#include <QRadioButton>
#include <qformlayout.h>
#include "window.h"
#include "ui_mainwindow.h"
#include "videowindow.h"
#include "sliderwidget.h"
#include "cameraCalibration/cameraCalib.cpp"
#include "filters.cpp"

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
    void Dilation(int, void*);

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionMean_triggered();

//    void on_actionSet_Kernel_Size_triggered();

    void on_actionMedian_triggered();

    void on_actionGaussian_triggered();

    void on_actionSalt_and_Pepper_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_focusChanged();

    void on_actionOpen_Camera_triggered();

    void on_actionFlip_Horizontally_triggered();

    void on_actionFlip_Vertically_triggered();

    void on_actionRotate_Clockwise_triggered();

    void on_actionRotate_Anti_Clockwise_triggered();

    void on_actionHistogram_Equalization_triggered();

    void on_actionInvert_triggered();

    void on_actionCanny_triggered();

    void on_actionSobel_triggered();

    void on_actionLaplacian_triggered();

    void on_actionDilation_triggered();

    void on_actionErosion_triggered();

    void on_actionOpening_triggered();

    void on_actionClosing_triggered();

    void on_actionUniform_triggered();

    void on_actionCamera_Calibration_triggered();

    void on_actionConvolution_Mask_triggered();

    void on_actionHistogram_triggered();

    void on_actionSIFT_triggered();

    void on_actionSURF_triggered();

    void on_actionHarris_triggered();

    void on_actionOpen_Video_triggered();

    void on_actionDraw_Epipolar_Lines_triggered();

    void on_actionMatch_Features_triggered();

    void on_actionFAST_triggered();

    void on_actionAdd_Logo_triggered();



    void on_actionStitch_Images_triggered();

    void on_actionHough_Transformaton_Circles_triggered();

    void on_actionHough_transformation_triggered();

    void on_actionCompute_Fundamental_Matrix_triggered();

    void on_actionCompute_Homography_triggered();

    void on_actionScale_Image_triggered();

    void on_actionAbout_triggered();

    void on_actionConvert_to_Binary_triggered();

    void on_actionConvert_To_Grayscale_triggered();

    void on_actionGaussian_noise_triggered();

protected:
    QVector<Window*> windowList;
    Ui::MainWindow *ui;

    Window* activeWindow;
};

#endif // MAINWINDOW_H
