//
// Created by hasan on 5/11/17.
//

#include "Find.h"

int Find::run() {
    VideoCapture cap(0);
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
    int angle=0;
    while (1)
    {
        // Reads frame from camera
        cap>>frame;
        // converts rgb to grayscale that reading image from camera
        cvtColor(frame,gray2,COLOR_BGR2GRAY);
        // Loses   noises
        GaussianBlur(gray2, gray, Size(7,7),0,0,BORDER_DEFAULT);
        medianBlur(gray2, gray, 7);
        // Sobel Edge Detection
        Sobel( gray, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );
        Sobel( gray, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edge );

        // Converts grayscale to binary image
        threshold( edge, thres, 100, 255,THRESH_OTSU );
        // Closing image for create connected component
        dilate( thres, dilation, element );
        morphologyEx(thres,dilation,MORPH_CLOSE,element);

        // Find contours
        findContours( dilation, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


        vector<vector<Point> > contours_poly( contours.size() );
        vector<RotatedRect> minRect( contours.size() );
        vector<RotatedRect> minEllipse( contours.size() );
        for( int i = 0; i < contours.size(); i++ ) {
            approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
            minRect[i] = minAreaRect(Mat(contours[i]));
            if( contours[i].size() > 5 )
                minEllipse[i] = fitEllipse( Mat(contours[i]) );
        }

        // Draws contours and rectangles
        for( int i = 0; i< contours.size(); i++ )
        {
            drawContours( thres, contours_poly, i, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
            if (minRect[i].size.area() > 10000) {
                Point2f rect_points[4];
                minRect[i].points(rect_points);
                for (int j = 0; j < 4; j++)
                    line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
                angle = calculateAngle(minRect[i]);
              //  if ((minEllipse[0].center.y - minEllipse[1].center.y ) > 0 )
                //    angle += 180;
                putText(frame,"Angle : " + to_string(angle),Point(100,150),FONT_HERSHEY_COMPLEX,2,Scalar(0,0,255),1,LINE_AA);
            }
        }


        imshow("thres", thres);
        //imshow("live", dilation);
        imshow("frame", frame);
        if (waitKey(30) == 'q') break;
    }
    return angle;
}

Find::Find() {

}

int Find::calculateAngle (RotatedRect minRect)   {
    Point2f rect_points[4];
    minRect.points(rect_points);
    float  angle = minRect.angle; // angle

    // read center of rotated rect
    cv::Point2f center = minRect.center; // center

    // choose the longer edge of the rotated rect to compute the angle
    cv::Point2f edge1 = cv::Vec2f(rect_points[1].x, rect_points[1].y) - cv::Vec2f(rect_points[0].x, rect_points[0].y);
    cv::Point2f edge2 = cv::Vec2f(rect_points[2].x, rect_points[2].y) - cv::Vec2f(rect_points[1].x, rect_points[1].y);

    cv::Point2f usedEdge = edge1;
    if(cv::norm(edge2) > cv::norm(edge1))
        usedEdge = edge2;

    cv::Point2f reference = cv::Vec2f(1,0); // horizontal edge


    angle = 180.0f/CV_PI * acos((reference.x*usedEdge.x + reference.y*usedEdge.y) / (cv::norm(reference) *cv::norm(usedEdge)));
    return  angle;
}