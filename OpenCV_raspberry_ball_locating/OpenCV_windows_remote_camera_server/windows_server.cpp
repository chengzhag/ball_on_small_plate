/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  基于OpenCV和Winsock的图像传输（接收）
//
//  By 彭曾 , at CUST, 2016.08.06
//
//  website: www.pengz0807.com  email: pengz0807@163.com
//
//M*/



#include <iostream>
#include "WinsockMatTransmissionServer.h"  

using namespace std;
using namespace cv;

int main()
{
	WinsockMatTransmissionServer socketMat;
	socketMat.begin(6666);

	cv::Mat image;
	while (1)
	{
		if (socketMat.receive(image) > 0)
		{
			cv::imshow("", image);
			cv::waitKey(1);
		}
		else
		{
			break;
		}
	}

	socketMat.disconnect();
	destroyWindow("");
	return 0;
}