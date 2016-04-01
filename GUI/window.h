#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFocusEvent>
#include "thirdParty/cvMatToQImage.cpp"

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(const QString file);
    explicit Window(const cv::Mat im);
    explicit Window(const QImage im);
    explicit Window();
    ~Window();
    void setImage(QImage im);
    void setImage(cv::Mat im);
    cv::Mat getImageMat() const;
    QImage getImage() const;
//    void closeWindow();
    void save(QString file = QString());
    QString getFileName() const;
    int i;
private slots:
    bool event(QEvent *e);

signals:
    void windowActivated();
    void windowClosed();

private:
    QImage imageQ;
    cv::Mat imageMat;
    QLabel* imageLabel;
    QString filename;
    void updateLabel();
};

#endif // WINDOW_H
