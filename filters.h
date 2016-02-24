#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Filters
{

public:
    Filters();
    Filters(const cv::Mat &image);
    ~Filters();

    cv::Mat filterMean(cv::Mat img, int kern = 3);

private:

};


#endif // FILTERS_H
