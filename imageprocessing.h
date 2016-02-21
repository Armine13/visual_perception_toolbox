#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>


class ImageProcessing
{
public:
    ImageProcessing(const cv::Mat &image);
    ~ImageProcessing();

    cv::Mat filterMean(int kern = 3);

private:
    cv::Mat img;
};

#endif // IMAGEPROCESSING_H
