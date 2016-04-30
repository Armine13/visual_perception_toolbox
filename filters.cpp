class Blur {
public:
    Blur(int k) : kern(k) {}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        blur(input, output, cv::Size( kern, kern), cv::Point(-1,-1) );
    }
private:
    int kern;
};
class MedianBlur {
public:
    MedianBlur(int k) : kern(k) {}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        medianBlur(input, output, kern);
    }
private:
    int kern;
};
class GaussianBlurFunctor{
public:
    GaussianBlurFunctor(cv::Size sz,int sX,int sY) : size(sz), sigmaX(sX), sigmaY(sY) {}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        GaussianBlur(input, output, size,sigmaX, sigmaY);
    }
private:
    cv::Size size;
    int sigmaX;
    int sigmaY;
};
class CannyFunctor{
public:
    CannyFunctor(int ts, int r, int k_size) : lowThreshold(ts), ratio(r), kernel_size(k_size){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        cvtColor(input, output, CV_BGR2GRAY );
        blur(output, output, Size(3,3) );
        Canny( output, output, lowThreshold, lowThreshold*ratio, kernel_size );
        cvtColor( output, output, CV_GRAY2BGR);
    }
private:
    int lowThreshold;
    int ratio;
    int kernel_size;
};
class SobelFunctor{
public:
    SobelFunctor(int s, int d) : scale(s), delta(d){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        cvtColor( input, output, CV_BGR2GRAY );
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;

        Sobel(output, grad_x, CV_16S, 0, 1, 3, scale, delta, BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );

        Sobel(output, grad_y, CV_16S, 0, 1, 3, scale, delta, BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );

        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, output);
        cvtColor( output, output, CV_GRAY2BGR);
    }
private:
    int scale;
    int delta;
};
class LaplacianFunctor{
public:
    LaplacianFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        cvtColor( input, output, CV_RGB2GRAY );
        Laplacian( output, output, CV_16S, 3, 1, 0, BORDER_DEFAULT);
        convertScaleAbs( output, output);
        cvtColor( output, output, CV_GRAY2BGR);
    }
};
class DilateFunctor{
public:
    DilateFunctor(cv::Mat el): element(el){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        dilate(input, output, element );
    }
    cv::Mat element;
};

class ErodeFunctor{
public:
    ErodeFunctor(cv::Mat el): element(el){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        erode(input, output, element );
    }
    cv::Mat element;
};
class OpeningFunctor{
public:
    OpeningFunctor(cv::Mat el): element(el){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        erode(input, output, element );
        dilate(output, output, element );
    }
    cv::Mat element;
};
class ClosingFunctor{
public:
    ClosingFunctor(cv::Mat el): element(el){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        dilate(input, output, element );
        erode(output, output, element );
    }
    cv::Mat element;
};
class SaltPaperFunctor{
public:
    SaltPaperFunctor(int prob): p(prob){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat saltpepper_noise = Mat::zeros(input.rows, input.cols,CV_8U);
        randu(saltpepper_noise, 0, 255);

        Mat black = saltpepper_noise < 255*p/200;
        Mat white = saltpepper_noise > 255-225*p/200;

        output = input.clone();
        output.setTo(255,white);
        output.setTo(0,black);
    }
private: int p;
};
class GaussianNoiseFunctor{
public:
    GaussianNoiseFunctor(int s): sigma(s){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {

        output = Mat(input.size(),CV_8UC3);
        Mat mGaussian_noise = Mat(input.size(),CV_16SC3);
        randn(mGaussian_noise, 0, sigma);

        for (int Rows = 0; Rows < input.rows; Rows++)
        {
            for (int Cols = 0; Cols < input.cols; Cols++)
            {
                Vec3b Source_Pixel= input.at<Vec3b>(Rows,Cols);
                Vec3b &Des_Pixel= output.at<Vec3b>(Rows,Cols);
                Vec3s Noise_Pixel= mGaussian_noise.at<Vec3s>(Rows,Cols);

                for (int i = 0; i < 3; i++)
                {
                    int Dest_Pixel= Source_Pixel.val[i] + Noise_Pixel.val[i];
                    Dest_Pixel = (Dest_Pixel < 0)? Dest_Pixel = 0: Dest_Pixel;
                    Dest_Pixel = (Dest_Pixel > 255)? Dest_Pixel = 255: Dest_Pixel;
                    Des_Pixel.val[i]= Dest_Pixel;
                }
            }
        }
    }
private:
    int sigma;
};

class FlipFunctor{
public:
    FlipFunctor(int d):dir(d){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        flip(input, output, dir);
    }
private:
    int dir;
};
class RotateFunctor{
public:
    RotateFunctor(int d):dir(d){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat img = input;
        transpose(img, img);
        flip(img, output, dir);
    }
private:
    int dir;
};
class InvertFunctor{
public:
    InvertFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        output =  cv::Scalar::all(255) - input;
    }
};
class SiftFunctor{
public:
    SiftFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat img;
        //        if (input.channels() == 3)
        cvtColor(input, img, CV_BGR2GRAY );
        //        else img = input;

        SIFT sift;
        vector<KeyPoint> key_points;

        Mat descriptors;
        sift(img, Mat(), key_points, descriptors);

        drawKeypoints(img, key_points, output);
    }
};
class FastFunctor{
public:
    FastFunctor(int ts):threshold(ts){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat img;
        //        if (input.channels() == 3)
        cvtColor(input, img, CV_BGR2GRAY );
        //        else img = input;

        vector<KeyPoint> key_points;

        Mat descriptors;

        FAST(img, key_points, threshold, true);
        drawKeypoints(img, key_points, output);
    }
private:
    int threshold;
};
class SurfFunctor{
public:
    SurfFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat img;
        //        if (input.channels() == 3)
        cvtColor(input, img, CV_BGR2GRAY );
        //        else img = input;

        SURF surf;
        vector<KeyPoint> key_points;

        Mat descriptors;
        surf(img, Mat(), key_points, descriptors);

        drawKeypoints(img, key_points, output);
    }
};
class HarrisFunctor{
public:
    HarrisFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat src_gray;
        int thresh = 150;

                if (input.channels() == 3)
        cvtColor( input, src_gray, CV_BGR2GRAY );
                else src_gray = input;
        Mat dst, dst_norm, dst_norm_scaled;
        dst = Mat::zeros( input.size(), CV_32FC1 );

        // Detector parameters
        int blockSize = 2;
        int apertureSize = 3;
        double k = 0.04;

        // Detecting corners
        cornerHarris( src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

        // Normalizing
        normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
        convertScaleAbs( dst_norm, dst_norm_scaled );

        output = src_gray;
        // Drawing a circle around corners
        for( int j = 0; j < dst_norm.rows ; j++ )
        { for( int i = 0; i < dst_norm.cols; i++ )
            {
                if( (int) dst_norm.at<float>(j,i) > thresh )
                {
                    circle( output, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );
                }
            }
        }
    }
};
class AddLogoFunctor{
public:
    AddLogoFunctor(cv::Mat log, int loc): location(loc){log.copyTo(logo);}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        input.copyTo(output);

        if (logo.size().width > input.size().width  || logo.size().height > input.size().height)
            return;

        if (location == 1)
            logo.copyTo(output.rowRange(0, logo.rows).colRange(0, logo.cols));
        if (location == 2)
            logo.copyTo(output.rowRange(0, logo.rows).colRange(output.cols - logo.cols, output.cols));
        if (location == 3)
            logo.copyTo(output.rowRange(output.rows - logo.rows, output.rows).colRange(0, logo.cols));
        if (location == 4)
            logo.copyTo(output.rowRange(output.rows - logo.rows, output.rows).colRange(output.cols - logo.cols, output.cols));
    }
private:
    cv::Mat logo;
    int location;
};
class HoughCirclesFunctor{
public:
    HoughCirclesFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        input.copyTo(output);

        Mat src_gray;

        cvtColor( input, src_gray, CV_BGR2GRAY );

        GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

        vector<Vec3f> circles;

        /// Apply the Hough Transform to find the circles
        HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, 5, 230, 70, 0, 0 );

        /// Draw the circles detected
        for( size_t i = 0; i < circles.size(); i++ )
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( output, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            cv::circle( output, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }
    }
};
class HoughLinesFunctor{
public:
    HoughLinesFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        Mat src;
        input.copyTo(src);

        Mat dst, cdst;
           Canny(src, dst, 50, 200, 3);
           cvtColor(dst, cdst, CV_GRAY2BGR);

           vector<Vec2f> lines;
           // detect lines
           HoughLines(dst, lines, 1, CV_PI/180, 150, 0, 0 );

           // draw lines
           for( size_t i = 0; i < lines.size(); i++ )
           {
               float rho = lines[i][0], theta = lines[i][1];
               Point pt1, pt2;
               double a = cos(theta), b = sin(theta);
               double x0 = a*rho, y0 = b*rho;
               pt1.x = cvRound(x0 + 1000*(-b));
               pt1.y = cvRound(y0 + 1000*(a));
               pt2.x = cvRound(x0 - 1000*(-b));
               pt2.y = cvRound(y0 - 1000*(a));
               line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
           }

        cdst.copyTo(output);
    }
};


class ResizeFunctor{
public:
    ResizeFunctor(int h, int w): height(h), width(w) {}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        input.copyTo(output);
        cv::resize(output, output, Size(width, height));
    }
private:
    int height;
    int width;
};
class BinaryFunctor{
public:
    BinaryFunctor(int t): th(t){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        cvtColor( input, output, CV_BGR2GRAY );

        threshold(output, output, th, 255, 0);
        cvtColor( output, output, CV_GRAY2BGR);
    }
private:
    int th;
};

class GrayFunctor{
public:
    GrayFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        cvtColor(input, output, CV_BGR2GRAY);
        cvtColor(output, output, CV_GRAY2BGR);
    }
};
class HisteqFunctor{
public:
    HisteqFunctor(){}
    void operator()(const cv::Mat input, cv::Mat& output)
    {
        if(input.channels() >= 3)
        {
            Mat ycrcb;

            cvtColor(input, ycrcb, CV_BGR2YCrCb);

            vector<Mat> channels;
            split(ycrcb,channels);

            equalizeHist(channels[0], channels[0]);


            merge(channels,ycrcb);

            cvtColor(ycrcb,output,CV_YCrCb2BGR);
        }
        else
            equalizeHist( input, output);
    }
};
