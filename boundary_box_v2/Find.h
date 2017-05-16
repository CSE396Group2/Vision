//
// Created by hasan on 5/11/17.
//

#ifndef FIND_H
#define FIND_H


#include <iostream>
#include <cv.h>
#include <opencv2/videoio.hpp>
#include <cv.hpp>

using namespace std;
using namespace cv;

class Find {
public:
    Find(){cap.open(0);};
    Find(int i){cap.open(i);};
    int calculateAngle (RotatedRect minRect);
    int run();

private:
    VideoCapture cap;
};


#endif //BOUNDARY_BOX_FIND_H
