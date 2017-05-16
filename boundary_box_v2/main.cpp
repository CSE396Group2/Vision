
#include <iostream>
#include <cv.h>
#include <opencv2/videoio.hpp>
#include <cv.hpp>
#include <highgui.h>
#include "Find.h"
using namespace std;
using namespace cv;


int main(int argc, char **argv)
{
    Find f(0);
    while (1)
    {
        cout<<f.run()<<endl;
        imshow("frame", f.live);
        if (waitKey(30) == 'q') break;
    }
}

