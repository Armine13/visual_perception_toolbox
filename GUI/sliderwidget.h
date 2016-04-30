#ifndef SLIDERWIDGET_H
#define SLIDERWIDGET_H

#include <QtGui>
#include <QDebug>
//#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>

class SliderWidget: public QWidget
{
    Q_OBJECT

public:
    SliderWidget(int minValue, int maxValue, int defaultValue, QString labelText = QString(),bool percents = false, QWidget *p = 0);
    void setText(QString str);
    int getValue() const;

public slots:
    void updateSliderBPos(int value);

private:
    QVBoxLayout *choiceBoxBLayout;
    QLabel *sliderPosB;
    QSlider *percentageSliderB;
    QLabel *textLabel;
    int value;
    bool showPercent;
signals:
    void updatedValue(int);
};
#endif // SLIDERWIDGET_H
