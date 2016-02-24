#include "filters.h"


Filters::Filters()
{}

Filters::~Filters()
{
}
cv::Mat Filters::filterMean(cv::Mat img, int kern)
{
    cv::Mat dst;

    blur( img, dst, cv::Size( kern, kern ), cv::Point(-1,-1) );
    return dst;
}

