#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "filters.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>


class ImageProcessing: public Filters
{
public:
    ImageProcessing();
    ~ImageProcessing();

    cv::Mat filterMean(const cv::Mat &image, int kern = 3);

private:
};

#endif // IMAGEPROCESSING_H
