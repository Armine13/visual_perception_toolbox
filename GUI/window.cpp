#include "window.h"

Window::Window(QString file) :
    QWidget()
{i = 0;
    filename = file;

    imageMat = cv::imread(filename.toStdString());

    assert(!imageMat.empty());

    imageQ = cvMatToQImage(imageMat);

    //Display image in label
    imageLabel = new QLabel(this);
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    updateLabel();
    show();
}
Window::Window() :
    QWidget()
{
    filename = "";

    imageQ = QImage(240, 320,QImage::Format_RGB32);
    imageQ.fill(Qt::white);
    imageMat = cv::Mat(imageQ.height(), imageQ.width(), CV_8UC4, imageQ.bits(), imageQ.bytesPerLine());//QImage to cv::Mat

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    updateLabel();
    show();
}
Window::Window(cv::Mat im) :
    QWidget()
{
    filename = "";

    imageMat = im;
    imageQ = cvMatToQImage(im);

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    updateLabel();
    show();
}
Window::Window(QImage im) :
    QWidget()
{
    filename = "";

    imageQ = im;
    imageMat = cv::Mat(im.height(), im.width(), CV_8UC4, im.bits(), im.bytesPerLine());//QImage to cv::Mat

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    updateLabel();
    show();
}
void Window::setImage(QImage im)
{
    imageQ = im;
    imageMat = cv::Mat(im.height(), im.width(), CV_8UC4, im.bits(), im.bytesPerLine());//QImage to cv::Mat
    updateLabel();
//    show();
}
void Window::setImage(Mat im)
{
    imageMat = im;//cv::mat
    imageQ = cvMatToQImage(im);//Convert mat to QImage
    updateLabel();
//    show();
}
void Window::updateLabel()
{
    imageLabel->setPixmap(QPixmap::fromImage(imageQ));

    // Adjust sizes of label to contained image
    imageLabel->setMinimumSize(imageQ.size());
    setMinimumSize(imageQ.size());
}

Window::~Window()
{
    qDebug()<<"window descructor ";
    this->deleteLater();
}
cv::Mat Window::getImageMat() const
{
    return imageMat;
}
QImage Window::getImage() const
{
    return imageQ;
}
void Window::save(QString file)
{
    if (!file.isEmpty())
        filename = file;

    imwrite(filename.toStdString(), imageMat);

}
QString Window::getFileName() const
{
    return filename;
}
bool Window::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate) {
        // window was activated
        emit windowActivated();
    }
    if (e->type() ==QEvent::Close)
        //window closed
        emit windowClosed();

    return QWidget::event(e);
}
