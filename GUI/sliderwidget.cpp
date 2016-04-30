#include "sliderwidget.h"

SliderWidget::SliderWidget(int minValue, int maxValue, int defaultValue, QString labelText, bool percents, QWidget *p)
    : QWidget(p)
{
    showPercent = percents;

    value = defaultValue;
    choiceBoxBLayout = new QVBoxLayout;
    setLayout(choiceBoxBLayout);

    if (showPercent) sliderPosB = new QLabel(QString::number(value).append('%'));
    else sliderPosB = new QLabel(QString::number(value));
    percentageSliderB = new QSlider(Qt::Horizontal);
    percentageSliderB->setMinimum(minValue);
    percentageSliderB->setMaximum(maxValue);
    percentageSliderB->setSliderPosition(defaultValue);
    connect(percentageSliderB,SIGNAL(valueChanged(int)),this,SLOT(updateSliderBPos(int)));
    textLabel = new QLabel(labelText);
    choiceBoxBLayout->addWidget(textLabel);
    choiceBoxBLayout->addWidget(percentageSliderB);
    choiceBoxBLayout->addWidget(sliderPosB);
}
void SliderWidget::updateSliderBPos(int val)
{
    value = val;
    if (showPercent) sliderPosB->setText(QString("%1%").arg(value));
    else sliderPosB->setText(QString("%1").arg(value));

    int updatedValue = value;
    emit updatedValue;
}
void SliderWidget::setText(QString str)
{
    textLabel->setText(str);
}
int SliderWidget::getValue() const
{
    return value;
}
