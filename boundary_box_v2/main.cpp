
#include <iostream>
#include <cv.h>
#include <opencv2/videoio.hpp>
#include <cv.hpp>

using namespace std;
using namespace cv;


int main(int argc, char **argv)
{
    VideoCapture cap(0);
    Mat frame, gray, edge, thres;

    /// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y,dilation;
    Mat element = getStructuringElement( MORPH_RECT,
                                         Size( 11,11 ),
                                         Point( 0, 0 ) );
    /// Apply the dilation operation

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    while (1)
    {


        cap>>frame;
        cvtColor(frame,frame,COLOR_BGR2GRAY);
        GaussianBlur(frame, gray, Size(7,7),0,0,BORDER_DEFAULT);
        medianBlur(frame, gray, 7);

        Sobel( gray, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );


        Sobel( gray, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );

        /// Total Gradient (approximate)
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edge );

        threshold( edge, thres, 100, 255,THRESH_OTSU );
        dilate( thres, dilation, element );
        morphologyEx(thres,dilation,MORPH_CLOSE,element);


        /// Find contours
        findContours( dilation, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        /// Approximate contours to polygons + get bounding rects and circles
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size() );
        vector<float>radius( contours.size() );

        for( int i = 0; i < contours.size(); i++ )
        { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
            minEnclosingCircle( contours_poly[i], center[i], radius[i] );
        }



        /// Draw polygonal contour + bonding rects + circles
        //Mat drawing = Mat::zeros( thres.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
        {

            drawContours( thres, contours_poly, i, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );

            if (boundRect[i].area() > 10000) {
                rectangle(thres, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 255), 2, 8, 0);
                rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 125, 0), 2, 8, 0);
            }
            //circle( drawing, center[i], (int)radius[i], Scalar(255,0,0), 2, 8, 0 );
        }


        imshow("thres", thres);
        //imshow("live", dilation);
        imshow("frame", frame);
        if (waitKey(30)>=0) break;
    }




}
