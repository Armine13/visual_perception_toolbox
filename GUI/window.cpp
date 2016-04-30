#include "window.h"

Window::Window(QString file, QMainWindow* parent) :
    QMainWindow(parent)
{
    filename = file;

    imageMat = cv::imread(filename.toStdString());

    assert(!imageMat.empty());

    imageQ = cvMatToQImage(imageMat);

    //Display image in label
    imageLabel = new QLabel(this);

    setWindowFlags(this->windowFlags() |Qt::Window);
    setMinimumSize(imageQ.size());
    updateLabel();
    show();
}
Window::Window(QMainWindow *parent) :
    QMainWindow(parent)
{
    filename = "";

    imageQ = QImage(240, 320,QImage::Format_RGB32);
    imageQ.fill(Qt::white);
    imageMat = cv::Mat(imageQ.height(), imageQ.width(), CV_8UC4, imageQ.bits(), imageQ.bytesPerLine());//QImage to cv::Mat

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);

    setWindowFlags(this->windowFlags() |Qt::Window);
    setMinimumSize(imageQ.size());
    updateLabel();
    show();
}
Window::Window(cv::Mat im, QMainWindow *parent) :
    QMainWindow(parent)
{
    filename = "";


    imageMat = im;
    imageQ = cvMatToQImage(im);

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);

    setWindowFlags(this->windowFlags() |Qt::Window);

    setMinimumSize(imageQ.size());
    updateLabel();
    show();
}
Window::Window(QImage im, QMainWindow *parent) :
    QMainWindow(parent)
{
    filename = "";


    imageQ = im;
    imageMat = cv::Mat(im.height(), im.width(), CV_8UC4, im.bits(), im.bytesPerLine());//QImage to cv::Mat

    assert(!imageMat.empty());

    //Display image in label
    imageLabel = new QLabel(this);

    setWindowFlags(this->windowFlags() |Qt::Window);
    setMinimumSize(imageQ.size());
    updateLabel();
    show();
}

void Window::setImage(Mat im)
{
    if (filterFunc != NULL)
    {
        filterFunc(im, imageMat);

    }
    else
        imageMat = im;//cv::mat

//    cvtColor(imageMat, imageMat, CV_BGR2RGB);
    imageQ = cvMatToQImage(imageMat);//Convert mat to QImage
    setMinimumSize(imageQ.size());

    updateLabel();
}

void Window::updateLabel()
{
    imageLabel->setPixmap(QPixmap::fromImage(imageQ));

    // Adjust sizes of label to contained image
    imageLabel->setMinimumSize(imageQ.size());
    this->adjustSize();
}

Window::~Window()
{
}
const Mat Window::getImageMat() const
{
    return imageMat;
}
const QImage Window::getImage() const
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
void Window::removeFilter()
{
    filterFunc = NULL;
}
