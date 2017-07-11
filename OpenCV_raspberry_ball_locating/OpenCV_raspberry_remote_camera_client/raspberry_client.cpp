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


using namespace cv;
using namespace std;

//#define STDIO_DEBUG
//#define SOCKET_SEND_IMAGE


int main(int argc, char **argv)
{
	///声明变量
	//摄像头
	raspicam::RaspiCam_Cv cam;
	//参数设置
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, cam.get(CV_CAP_PROP_FRAME_WIDTH) * 0.25);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, cam.get(CV_CAP_PROP_FRAME_HEIGHT) * 0.25);
	int imRawH = cam.get(CV_CAP_PROP_FRAME_HEIGHT),
		imRawW = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	Mat imRaw;
	//剪切平板位置图像
	float plateRegionHeight = 0.73;//, plateRegionWidth = 0.52;
	float plateRegionOffH = -0.04, plateRegionOffW = 0.052;
	Rect plateRegion(
		int(imRawW*(0.5 + plateRegionOffW) - imRawH*plateRegionHeight / 2),
		int(imRawH*(0.5 + plateRegionOffH - plateRegionHeight / 2)),
		imRawH*plateRegionHeight,
		imRawH*plateRegionHeight);
	imRawW = plateRegion.width;
	imRawH = plateRegion.height;
//	//二值化，轮廓检测
//	Mat imThresh;
//	const float resizeThresh = 1;
	
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

#ifdef SOCKET_SEND_IMAGE
	cout << "socket connecting..." << endl;
	SocketMatTransmissionClient socketMat;
	socketMat.begin("192.168.2.100");
#endif // SOCKET_SEND_IMAGE
	
	//初始化串口
	UartNum<int> uart;
	uart.begin();

	double timeStart = 0, timeEnd = 0;
	
//	//初始化阈值，固定二值化阈值，减轻后面计算负担
//	double threshBinary;
//	for (int i = 0; i < 30; i++)
//	{
//		cam.grab();
//		cam.retrieve(imRaw);
//		imRaw = imRaw(plateRegion);
//		threshBinary += threshold(imRaw, imRaw, 0, 255, CV_THRESH_OTSU); 
//	}
//	threshBinary /= 30;
//	threshBinary /= 0.8;
	
	
	while (1)
	{
		cam.grab();
		cam.retrieve(imRaw);

		if (imRaw.empty())
		{
			cout << "imRaw为空！！！" << endl;
			return 1;
		}
		
		imRaw = imRaw(plateRegion);
			
		
		/// 小球定位算法开始
		int pos[2];
//		resize(imRaw, imThresh, Size(0, 0), resizeThresh, resizeThresh, INTER_NEAREST);
//		threshold(imRaw, imThresh, threshBinary, 255, CV_THRESH_BINARY);
//		threshold(imThresh, imThresh, 0, 255, CV_THRESH_OTSU);
//		morphologyEx(imRaw, imProcess, CV_MOP_TOPHAT, element);
		morphologyEx(imRaw, imProcess, CV_MOP_ERODE, element);
//		medianBlur(imProcess, imProcess, 9);
		GaussianBlur(imRaw, imProcess, 
			Size(int(0.01*imRawH) * 2 + 1, int(0.01*imRawW) * 2 + 1), 
			int(0.01*imRawW) * 2 + 1, int(0.01*imRawW) * 2 + 1);
		//以小球半径的两倍为窗口长度

		
//		int imThreshH = imThresh.rows,
//			imThreshW = imThresh.cols;

////		轮廓提取
//		vector<vector<Point> > contours;
//		findContours(imThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//		
//  
//		if (contours.size() > 0)
//		{
//			//选出最大长度的轮廓 
//			double lengthMax = 0;  
//			int index = 0;  
//			for (int i = 0; i < contours.size(); i++)  
//			{  
//				if (contours[i].size() > lengthMax)  
//				{  
//					lengthMax = contours[i].size();  
//					index = i;  
//				}  
//			}  
//      
//			//多边形逼近 
//			approxPolyDP(contours[index], contours[index], imThreshH * 0.5, true);
		
#ifdef SOCKET_SEND_IMAGE
//			//最外围轮廓的显示  
//			drawContours(imThresh,
//				contours,
//				index,
//				Scalar(128),
//				4,
//				8);  
#endif // SOCKET_SEND_IMAGE

		
		Point ballPoint;
		double minBrightness;
		minMaxLoc(imProcess, &minBrightness, NULL, &ballPoint, NULL);
		if (minBrightness < 90)
		{
			pos[0] = ballPoint.x;
			pos[1] = ballPoint.y;
		}
		else
		{
			pos[0] = -1;
			pos[1] = -1;
		}//如果最小亮度过高，认为小球掉落
		
//			//矩形透视投影
//		
//			if (contours[index].size() == 4)
//			{
//				vector<Point2f> corners;
//			
//				for (int i = 0; i < 4; i++)
//				{
//					corners.push_back(Point2f(contours[index][i].x / resizeThresh, contours[index][i].y / resizeThresh));
//				}
//			
//				sortCorners(corners,
//					Point(imRawW / 2, imRawH / 2));
//			
//				Mat M = getPerspectiveTransform(corners, PerspectiveTransform);
//				warpPerspective(imRaw,
//					imTrans,
//					M,
//					Size(imTransW, imTransH),
//					INTER_NEAREST,
//					BORDER_CONSTANT,
//					Scalar(255));  
//			
//		//			morphologyEx(imTrans, imTrans, CV_MOP_OPEN, element);
//					//定位小球
//				Point ballPoint;
//				double minBrightness;
//				minMaxLoc(imTrans, &minBrightness, NULL, &ballPoint, NULL);
//				if (minBrightness < 50)
//				{
//					pos[0] = ballPoint.x;
//					pos[1] = ballPoint.y;
//				}
//				else
//				{
//					pos[0] = -1;
//					pos[1] = -1;
//				}//如果最小亮度过高，认为小球掉落
//			
//			}
//			else
//			{
//				pos[0] = -1;
//				pos[1] = -1;
//			}//如果矩形轮廓点数不为4，板下检测到亮度较高的异物
//		}
//		else
//		{
//			pos[0] = -1;
//			pos[1] = -1;
//		}//如果没有检测到轮廓，二值化算法失效

	
		
		
#ifdef STDIO_DEBUG
		//计算算法帧率
		cout << "fps: " << 1.0 / (timeEnd - timeStart)*(double)getTickFrequency()
				<< "\t" << pos[0] << " of " << imProcess.cols 
				<< "\t" << pos[1] << " of " << imProcess.rows
				<< "\t" << "brightness: " << minBrightness << endl;
//		unsigned char* posChar = (unsigned char*)pos;
//		for (int i = 0; i < 8; i++)
//		{
//			printf("%u ", posChar[i]);
//		}
//		cout << endl;
		timeStart = timeEnd;
		timeEnd = (double)getTickCount();
#endif // STDIO_DEBUG
		
		uart.sendNum(pos, 2);

		///小球定位算法结束
		
		
		
#ifdef SOCKET_SEND_IMAGE
		//发送图像，用于测试
		resize(imProcess, imSend, Size(0, 0), 0.2, 0.2, INTER_NEAREST);
//		threshold(imTrans, imSend, threshBinary, 255, CV_THRESH_BINARY);
		socketMat.transmit(imSend, 80);
#endif // SOCKET_SEND_IMAGE
		
		
	}
#ifdef SOCKET_SEND_IMAGE
	socketMat.disconnect();
#endif // SOCKET_SEND_IMAGE
	cam.release();
	return 0;
}




