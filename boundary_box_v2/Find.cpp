//
// Created by hasan on 5/11/17.
//

#include "Find.h"

int Find::run() {
    Mat frame, gray, gray2, edge, thres;
/// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y,dilation;
    Mat element = getStructuringElement( MORPH_RECT,
                                         Size( 11,11 ),
                                         Point( 0, 0 ) );
/// Apply the dilation operation
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    cap>>frame;

    cvtColor(frame,gray2,COLOR_BGR2GRAY);
    GaussianBlur(gray2, gray, Size(7,7),0,0,BORDER_DEFAULT);
    medianBlur(gray2, gray, 7);

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
    vector<RotatedRect> minRect( contours.size() );

/// Find the rotated rectangles and ellipses for each contour
    vector<RotatedRect> minEllipse( contours.size() );

    for( int i = 0; i < contours.size(); i++ )
    { minRect[i] = minAreaRect( Mat(contours[i]) );
        if( contours[i].size() > 5 )
        { minEllipse[i] = fitEllipse( Mat(contours[i]) ); }
    }

    /// Draw contours + rotated rects + ellipses
    for( int i = 0; i< contours.size(); i++ )
    {
        if (minRect[i].size.area() > 10000) {

            int centerXRect,centerYRect,ellipseCenterX,ellipseCenterY;
            centerXRect = minRect[i].center.x;
            centerYRect = minRect[i].center.y;
            RotatedRect max1= minEllipse[0], max2 = minEllipse[1];
            for (int k = 0; k <minEllipse.size() ; ++k) {
                if(minEllipse[k].size.area() > max1.size.area())
                {
                    max2 = max1;
                    max1 = minEllipse[k];
                }
            }

            ellipseCenterX = max2.center.x;
            ellipseCenterY = max2.center.y;
            int x = ellipseCenterX - centerXRect;
            int y = ellipseCenterY - centerYRect;
            double angle = minRect[i].angle;

            if ( x > 0 && y < 0)    {
                // region 1
                angle *= -1;

            }
            else if ( x < 0 && y < 0)    {
                // region 2
                angle *= -1;
                angle += 90;

            }
            else if ( x < 0 && y > 0)    {
                // region 3
                angle *= -1;
                angle += 180;

            }
            else if ( x > 0 && y > 0)    {
                // region 4
                angle *= -1;
                angle += 270;

            }

            Point2f rect_points[4];
            minRect[i].points(rect_points);
            for (int j = 0; j < 4; j++)
                line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0,0,255), 1, 8);

            live = frame;
//          imwrite("Result.jpg",frame);
            return  angle;
        }
    }
    live = frame;
    return  -10;
}