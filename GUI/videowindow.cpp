#include "videowindow.h"

VideoWindow::VideoWindow(QMainWindow *parent, int d):
    Window(parent), cap(0)
{
    cap = VideoCapture(0);
    cap.open(0);
    if(!cap.isOpened())
    {
        qDebug()<<"CANNOT OPEN CAMERA"<<endl;
        videoOn = false;
        close();
        return;
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));
    if (d > 0) delay = d;
    timer->start(delay);
    videoOn = true;
    camOrVid = 0;
    file = QString();
}
VideoWindow::VideoWindow(int camera, QMainWindow *parent, int d):
    Window(parent)
{
    cap = VideoCapture(camera);
    cap.open(camera);
    if(!cap.isOpened())
    {
        qDebug()<<"CANNOT OPEN CAMERA"<<endl;
        videoOn = false;
        close();
    }
    if (d > 0) delay = d;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));
    timer->start(delay);
    videoOn = true;
    camOrVid = 0;
    file = QString();
}
VideoWindow::VideoWindow(QString filename, QMainWindow *parent, int d):
    Window(parent)
{
    cap = VideoCapture(filename.toStdString());
    cap.open(filename.toStdString());
    if(!cap.isOpened())
    {
        qDebug()<<"CANNOT OPEN CAMERA"<<endl;
        videoOn = false;
        close();
        return;
    }
    if (d > 0) delay = d;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));
    timer->start(delay);
    videoOn = true;
    camOrVid = 1;
    file = filename;
}
VideoWindow::~VideoWindow()
{
    this->deleteLater();
    timer->stop();
    delete timer;
}
void VideoWindow::updateLabel()
{
    Mat frame;

    cap >> frame;

    if(frame.empty())
    {
        if (camOrVid && !file.isEmpty())
        {

            cap = VideoCapture(file.toStdString());
            cap.open(file.toStdString());
            if(!cap.isOpened())
            {
                qDebug()<<"CANNOT OPEN CAMERA"<<endl;
                videoOn = false;
                close();
                return;
            }
        }

//        timer->stop();
        videoOn = false;
        return;
    }

    setImage(frame);
    imageLabel->setMinimumSize(imageQ.size());
    this->adjustSize();
}
bool VideoWindow::isVideoOn() const
{
    return videoOn;
}
void VideoWindow::setDelay(int d)
{
    delay = d;
}
bool VideoWindow::camera_or_video() const
{
    return camOrVid;
}
