#include "imageprocessing.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>

ImageProcessing::ImageProcessing(const cv::Mat& image)
{
    img = image;
}
ImageProcessing::~ImageProcessing()
{
}
cv::Mat ImageProcessing::filterMean(int kern)
{
//    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
//         { blur( src, dst, Size( i, i ), Point(-1,-1) );
//           if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }
    cv::Mat dst;
//    qDebug()<<img.size().height;
    blur( img, dst, cv::Size( kern, kern ), cv::Point(-1,-1) );
    return dst;
}

