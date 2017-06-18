#include <ctime>
#include <iostream>
#include <raspicam_cv.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "time.h"

using namespace cv;
using namespace std;
 
int main(int argc, char **argv)
{
	raspicam::RaspiCam_Cv cam;
	Mat image;
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, cam.get(CV_CAP_PROP_FRAME_WIDTH) * 0.2);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, cam.get(CV_CAP_PROP_FRAME_HEIGHT) * 0.2);
	if (!cam.open())
		return 1;
    
	const char szSourceWindow[] = "Camera";
	namedWindow(szSourceWindow, WINDOW_AUTOSIZE);
	double startT, endT; 
	for (;;)
	{
		startT = clock();
		RNG rng(12345);
		cam.grab();
		cam.retrieve(image);
		imshow(szSourceWindow, image);
//		Mat smallImage;
//		resize(image, smallImage, Size(), 0.1, 0.1);
//		imshow(szSourceWindow, smallImage);
		waitKey(100);
		endT = clock();
		cout << (endT - startT) / CLOCKS_PER_SEC << endl;
	}
	cam.release();
}

