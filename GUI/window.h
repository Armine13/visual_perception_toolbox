#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFocusEvent>
#include "thirdParty/cvMatToQImage.cpp"

#include <typeinfo>
#include <functional>

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(const QString file, QMainWindow* parent);
    explicit Window(const cv::Mat im, QMainWindow* parent);
    explicit Window(const QImage im, QMainWindow* parent);
    explicit Window(QMainWindow* parent);
    ~Window();
//    void setImage(QImage im);
    void setImage(cv::Mat im);
    const cv::Mat getImageMat() const;
    const QImage getImage() const;

    void save(QString file = QString());
    QString getFileName() const;

    template <typename FuncType>
    void applyFilter(FuncType func)
    {
        filterFunc = func;

        if (filterFunc != NULL)
        {
            Mat img;
            filterFunc(imageMat, img);

            imageMat = img;
            imageQ = cvMatToQImage(img);
            updateLabel();
        }
    }
    void removeFilter();
private slots:
    bool event(QEvent *e);
public slots:
    void updateLabel();

signals:
    void windowActivated();
    void windowClosed();

protected:
    QImage imageQ;
    cv::Mat imageMat;
    QLabel* imageLabel;
    QString filename;

    std::function<void(const cv::Mat, cv::Mat&)> filterFunc = NULL;
};

#endif // WINDOW_H
