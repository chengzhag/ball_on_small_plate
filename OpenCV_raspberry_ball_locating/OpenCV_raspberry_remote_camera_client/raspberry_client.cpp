#include <ctime>
#include <iostream>
#include <stdio.h>
#include <raspicam_cv.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

#include "SocketMatTransmissionClient.h"  

#include "my_opencv.h"
#include "my_math.h"
#include <algorithm>
#include "uart_num.h"

#include "wiringPi.h"
#include "wiringSerial.h"


using namespace cv;
using namespace std;

//#define STDIO_DEBUG
//#define SOCKET_SEND_IMAGE


int main(int argc, char **argv)
{
	const float targetFPS = 30;
	const float targetInterval = 1 / targetFPS;
	piHiPri(99);//最高优先级
	///声明变量
#ifdef SOCKET_SEND_IMAGE
	cout << "socket connecting..." << endl;
	SocketMatTransmissionClient socketMat;
	socketMat.begin("192.168.2.100");
#endif // SOCKET_SEND_IMAGE
	//摄像头
	raspicam::RaspiCam_Cv cam;
	//参数设置
	float imRawFactor = 0.25;
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, cam.get(CV_CAP_PROP_FRAME_WIDTH) * imRawFactor);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, cam.get(CV_CAP_PROP_FRAME_HEIGHT) * imRawFactor);
	int imRawH = cam.get(CV_CAP_PROP_FRAME_HEIGHT),
		imRawW = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	Mat imRaw;
	
	//畸变矫正
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	cameraMatrix.at<double>(0, 0) = 6.581772704460833e02*imRawFactor;
	cameraMatrix.at<double>(0, 1) = 0.749911819487793*imRawFactor;
	cameraMatrix.at<double>(0, 2) = 6.452732528089651e02*imRawFactor;
	cameraMatrix.at<double>(1, 1) = 6.566540417566417e02*imRawFactor;
	cameraMatrix.at<double>(1, 2) = 4.371415328611994e02*imRawFactor;
	
	Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
	distCoeffs.at<double>(0, 0) = -0.285784464085280;
	distCoeffs.at<double>(1, 0) = 0.064227650409763;
	distCoeffs.at<double>(2, 0) = -7.378200021153921e-04;
	distCoeffs.at<double>(3, 0) = -4.368607112562313e-04;
	distCoeffs.at<double>(4, 0) = 0;
	
	Mat map1, map2;
	Size imRawSize(imRawW, imRawH);
	initUndistortRectifyMap(cameraMatrix,
		distCoeffs,
		Mat(),
		getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imRawSize, 1, imRawSize, 0),
		imRawSize,
		CV_16SC2,
		map1,
		map2);
	
	//剪切平板位置图像
	float plateRegionHeight = 0.67;//, plateRegionWidth = 0.52;
	float plateRegionOffH = -0.043, plateRegionOffW = 0.03;
	Rect plateRegion(
		int(imRawW*(0.5 + plateRegionOffW) - imRawH*plateRegionHeight / 2),
		int(imRawH*(0.5 + plateRegionOffH - plateRegionHeight / 2)),
		imRawH*plateRegionHeight,
		imRawH*plateRegionHeight);
	imRawW = plateRegion.width;
	imRawH = plateRegion.height;
	
	//平板尺寸信息
	const float maxX = 200, maxY = 200;
	
	
#ifdef SOCKET_SEND_IMAGE
	//发送图像，用于测试
	Mat imSend;
#endif // SOCKET_SEND_IMAGE
	

	
	///预处理
	Mat imProcess;
	const int structElementSize = 1;
	Mat element = getStructuringElement(MORPH_ELLIPSE,  
		Size(2*structElementSize + 1, 2*structElementSize + 1),  
		Point(structElementSize, structElementSize));
	
	
	///初始化
	//初始化连接
	if (!cam.open())
		return 1;
	
	//初始化串口
	UartNum<float> uart;
	uart.begin();
	
	//初始化阈值，固定二值化阈值，减轻后面计算负担
	double threshBinary = 0;
	for (int i = 0; i < 30; i++)
	{
		cam.grab();
		cam.retrieve(imRaw);
		if (imRaw.empty())
		{
			cout << "imRaw为空！！！" << endl;
			return 1;
		}
		remap(imRaw, imRaw, map1, map2, INTER_LINEAR);//INTER_NEAREST
		imRaw = imRaw(plateRegion);
		
		double minBrightness;
		minMaxLoc(imRaw, &minBrightness, NULL, NULL, NULL);
		threshBinary += minBrightness; 
		
#ifdef SOCKET_SEND_IMAGE
		//发送图像，用于测试
//		threshold(imTrans, imSend, threshBinary, 255, CV_THRESH_BINARY);
		socketMat.transmit(imRaw, 80);
#endif // SOCKET_SEND_IMAGE
	}
	threshBinary /= 30;
	threshBinary -= 5;

	double timeStart = 0, timeEnd = 0;

	
	
	while (1)
	{
		cam.grab();
		cam.retrieve(imRaw);

		if (imRaw.empty())
		{
			cout << "imRaw为空！！！" << endl;
			return 1;
		}
		
		remap(imRaw, imRaw, map1, map2, INTER_LINEAR);//INTER_NEAREST
		imRaw = imRaw(plateRegion);
			
		
		/// 小球定位算法开始
		float pos[2];
//		resize(imRaw, imThresh, Size(0, 0), resizeThresh, resizeThresh, INTER_NEAREST);
		threshold(imRaw, imProcess, threshBinary, 255, CV_THRESH_BINARY_INV);
//		threshold(imThresh, imThresh, 0, 255, CV_THRESH_OTSU);
//		morphologyEx(imRaw, imProcess, CV_MOP_TOPHAT, element);
		morphologyEx(imProcess, imProcess, CV_MOP_DILATE, element);
//		medianBlur(imProcess, imProcess, 9);
//		GaussianBlur(imRaw, imProcess, 
//			Size(int(0.01*imRawH) * 2 + 1, int(0.01*imRawW) * 2 + 1), 
//			int(0.01*imRawW) * 2 + 1, int(0.01*imRawW) * 2 + 1);
		//以小球半径的两倍为窗口长度
		
		//计算二值图形的矩特征
		Moments mu=moments(imProcess,true);
		Point2f ballPoint(mu.m10 / mu.m00, mu.m01 / mu.m00);
		pos[0] = ballPoint.x;
		pos[1] = ballPoint.y;
	
		
////		轮廓提取
//		vector<vector<Point> > contours;
//		findContours(imProcess, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		
//		Point ballPoint;
//		double minBrightness;
//		minMaxLoc(imProcess, &minBrightness, NULL, &ballPoint, NULL);
//		if (minBrightness < 30)
//		{
//			pos[0] = ballPoint.x;
//			pos[1] = ballPoint.y;
//		}
//		else
//		{
//			pos[0] = -1;
//			pos[1] = -1;
//		}//如果最小亮度过高，认为小球掉落

	
		//标准化坐标单位mm
		pos[0] = pos[0]*maxX / imProcess.rows;
		pos[1] = pos[1]*maxY / imProcess.cols;
		
		
#ifdef STDIO_DEBUG
		timeEnd = (double)getTickCount();
		//计算算法帧率
		cout << "fps: " << 1.0 / (timeEnd - timeStart)*(double)getTickFrequency()
				<< "\t" << pos[0] << " of " << maxX 
				<< "\t" << pos[1] << " of " << maxX
				<< "\t" << "thres: " << threshBinary 
				<< endl;
		timeStart = timeEnd;
#endif // STDIO_DEBUG

		uart.sendNum(pos, 2);

		///小球定位算法结束
		
		
		
#ifdef SOCKET_SEND_IMAGE
		//发送图像，用于测试
		resize(imProcess, imSend, Size(0, 0), 1, 1, INTER_NEAREST);
//		threshold(imTrans, imSend, threshBinary, 255, CV_THRESH_BINARY);
		socketMat.transmit(imSend, 80);
#endif // SOCKET_SEND_IMAGE
		
		//判断单片机发来的关机指令或按键发来的关机指令
		int fd = uart.getFd();
		char c;
		if (int num = serialDataAvail(fd) > 0)
		{
			for (int i = 0; i < num; i++)
			{
				c = serialGetchar(fd);
			}
		}
		if (c == 's')
		{
			system("sudo shutdown -h now");
		}
		else if (c == 'k')
		{
			system("sudo killall OpenCVCameraDemo");
		}
		
		
	}
#ifdef SOCKET_SEND_IMAGE
	socketMat.disconnect();
#endif // SOCKET_SEND_IMAGE
	cam.release();
	return 0;
}




