#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H
#include <QTimer>
#include "window.h"

class VideoWindow : public Window
{
    Q_OBJECT
public:
    VideoWindow(QMainWindow* parent, int d = 15);
    VideoWindow(int camera, QMainWindow* parent,int d = 15);
    VideoWindow(QString filename, QMainWindow* parent,int d = 15);
    ~VideoWindow();
    bool isVideoOn() const;
    void setDelay(int d);
    bool camera_or_video() const;
public slots:
    void updateLabel();

private:
    QTimer * timer;
    VideoCapture cap;
    bool camOrVid;
    bool videoOn;
    int delay;
    QString file;
};

#endif // VIDEOWINDOW_H

//ref
//http://stackoverflow.com/questions/14729417/open-webcamera-with-opencv-and-show-it-with-qlabel-white-window
