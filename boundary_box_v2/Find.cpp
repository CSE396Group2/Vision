//
// Created by hasan on 5/11/17.
//

#include "Find.h"

void Find::run() {
    VideoCapture cap(0);
    Mat frame, gray, edge, thres,blurred;

    // This variables used for closing operation
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y,dilation;
    Mat element = getStructuringElement( MORPH_RECT,Size( 11,11 ), Point( 0, 0 ) );

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    while (1)
    {
        // reads frame from camera
        cap>>frame;

        // RGB to Gray color conversion
        cvtColor(frame,gray,COLOR_BGR2GRAY);
        // 7x7 median filter
        medianBlur(gray, blurred, 7);
        //GaussianBlur(frame, gray, Size(7,7),0,0,BORDER_DEFAULT);

        // Sobel Edge Detection
        Sobel( blurred, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );
        Sobel( blurred, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edge );

        // Gray to binary color conversion
        threshold( edge, thres, 100, 255,THRESH_OTSU );
        // closing operation
        dilate( thres, dilation, element );
        morphologyEx(thres,dilation,MORPH_CLOSE,element);


        /// Find contours
        findContours( dilation, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        /// Approximate contours to polygons + get bounding rects and circles
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size() );
        vector<float>radius( contours.size() );
        vector<RotatedRect> ellipse( contours.size() );
        // Boundary Box Drawing
        for( int i = 0; i < contours.size(); i++ )  {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        /// Draw polygonal contour + bonding rects + circles
        for( int i = 0; i< contours.size(); i++ )
        {
            drawContours( thres, contours_poly, i, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
            if (boundRect[i].area() > 10000) {
                rectangle(thres, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 255, 255), 2, 8, 0);
                rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2, 8, 0);
                putText(frame, "x : " + to_string(boundRect[i].x) + " y : " + to_string(boundRect[i].y),Point(20,20),
                        FONT_HERSHEY_TRIPLEX,1,Scalar(255,0,0),4,LINE_AA);
                putText(thres, "x : " + to_string(boundRect[i].x) + " y : " + to_string(boundRect[i].y),Point(20,20),
                        FONT_HERSHEY_TRIPLEX,1,Scalar(255,255,255),4,LINE_AA);
            }
        }
        // Calculate Angle
        
        imshow("Binary", thres);
        imshow("Live", frame);
        moveWindow("Live",650,120);
        moveWindow("Binary",0,120);
        if (isalnum(waitKey(30)))
            break;
        // if (waitKey(30)>=0) break;
    }
}

Find::Find() {

}
