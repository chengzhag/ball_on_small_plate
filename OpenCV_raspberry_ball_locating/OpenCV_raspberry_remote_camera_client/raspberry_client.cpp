#include <ctime>
#include <iostream>
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



void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;
 
	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}
 
	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
 
	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}


int main(int argc, char **argv)
{
	///声明变量
	//摄像头
	raspicam::RaspiCam_Cv cam;
	//参数设置
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, cam.get(CV_CAP_PROP_FRAME_WIDTH) * 0.2);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, cam.get(CV_CAP_PROP_FRAME_HEIGHT) * 0.2);
	const int imRawH = cam.get(CV_CAP_PROP_FRAME_HEIGHT),
		imRawW = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	Mat imRaw;
	//二值化，轮廓检测
	Mat imThresh;
	const float resizeThresh = 1;
	//矩形透视投影
	const float resizeTrans = 0.5;
	const int imTransH = imRawH * resizeTrans,
				imTransW = imRawW * resizeTrans;
	Mat imTrans(imRawH,
		imRawW,
		CV_8UC1);
	const int cropPixels = imTransH * 0.05;
	//透视变换后的顶点  
	vector<Point2f> PerspectiveTransform;
	PerspectiveTransform.push_back(Point(-cropPixels, -cropPixels));  
	PerspectiveTransform.push_back(Point(imTransW - 1 + cropPixels, -cropPixels));  
	PerspectiveTransform.push_back(Point(imTransW - 1 + cropPixels, imTransH - 1 + cropPixels));  
	PerspectiveTransform.push_back(Point(-cropPixels, imTransH - 1 + cropPixels));  
	
#ifdef SOCKET_SEND_IMAGE
	//发送图像，用于测试
	Mat imSend;
#endif // SOCKET_SEND_IMAGE
	

	
	///预处理
//	const int structElementSize = 2;
//	Mat element = getStructuringElement(MORPH_ELLIPSE,  
//		Size(2*structElementSize + 1, 2*structElementSize + 1),  
//		Point(structElementSize, structElementSize));
	
	
	
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
	
	//初始化阈值，固定二值化阈值，减轻后面计算负担
	double threshBinary;
	for (int i = 0; i < 30; i++)
	{
		cam.grab();
		cam.retrieve(imRaw);
		threshBinary += threshold(imRaw, imRaw, 0, 255, CV_THRESH_OTSU); 
	}
	threshBinary /= 30;
	
	while (1)
	{
		cam.grab();
		cam.retrieve(imRaw);

		if (imRaw.empty())
		{
			cout << "imRaw为空！！！" << endl;
			return 1;
		}
			
		
		/// 小球定位算法开始
		int pos[2];
		resize(imRaw, imThresh, Size(0, 0), resizeThresh, resizeThresh, INTER_NEAREST);
		threshold(imThresh, imThresh, threshBinary, 255, CV_THRESH_BINARY);
//		threshold(imThresh, imThresh, 0, 255, CV_THRESH_OTSU);
//		morphologyEx(imThresh, imThresh, CV_MOP_OPEN, element);

		
		int imThreshH = imThresh.rows,
			imThreshW = imThresh.cols;

//		轮廓提取
		vector<vector<Point> > contours;
		findContours(imThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		
  
		if (contours.size() > 0)
		{
			//选出最大长度的轮廓 
			double lengthMax = 0;  
			int index = 0;  
			for (int i = 0; i < contours.size(); i++)  
			{  
				if (contours[i].size() > lengthMax)  
				{  
					lengthMax = contours[i].size();  
					index = i;  
				}  
			}  
      
			//多边形逼近 
			approxPolyDP(contours[index], contours[index], imThreshH * 0.3, true);
		
#ifdef SOCKET_SEND_IMAGE
			//最外围轮廓的显示  
			drawContours(imThresh,
				contours,
				index,
				Scalar(128),
				4,
				8);  
#endif // SOCKET_SEND_IMAGE

		
			//矩形透视投影
		
			if (contours[index].size() == 4)
			{
				vector<Point2f> corners;
			
				for (int i = 0; i < 4; i++)
				{
					corners.push_back(Point2f(contours[index][i].x / resizeThresh, contours[index][i].y / resizeThresh));
				}
			
				sortCorners(corners,
					Point(imRawW / 2, imRawH / 2));
			
				Mat M = getPerspectiveTransform(corners, PerspectiveTransform);
				warpPerspective(imRaw,
					imTrans,
					M,
					Size(imTransW, imTransH),
					INTER_NEAREST,
					BORDER_CONSTANT,
					Scalar(255));  
			
		//			morphologyEx(imTrans, imTrans, CV_MOP_OPEN, element);
					//定位小球
				Point ballPoint;
				double minBrightness;
				minMaxLoc(imTrans, &minBrightness, NULL, &ballPoint, NULL);
				if (minBrightness < 50)
				{
					pos[0] = ballPoint.x;
					pos[1] = ballPoint.y;
				}
				else
				{
					pos[0] = -1;
					pos[1] = -1;
				}//如果最小亮度过高，认为小球掉落
			
			}
			else
			{
				pos[0] = -1;
				pos[1] = -1;
			}//如果矩形轮廓点数不为4，板下检测到亮度较高的异物
		}
		else
		{
			pos[0] = -1;
			pos[1] = -1;
		}//如果没有检测到轮廓，二值化算法失效

	
		
		
#ifdef STDIO_DEBUG
		//计算算法帧率
		cout << "fps: " << 1.0 / (timeEnd - timeStart)*(double)getTickFrequency()
				<< " " << pos[0] << " " << pos[1] << endl;
		timeStart = timeEnd;
		timeEnd = (double)getTickCount();
#endif // STDIO_DEBUG
		
		uart.sendNum(pos, 2);

		///小球定位算法结束
		
		
		
#ifdef SOCKET_SEND_IMAGE
		//发送图像，用于测试
		resize(imTrans, imSend, Size(0, 0), 1, 1, INTER_NEAREST);
		socketMat.transmit(imSend, 90);
#endif // SOCKET_SEND_IMAGE
		
		
	}
#ifdef SOCKET_SEND_IMAGE
	socketMat.disconnect();
#endif // SOCKET_SEND_IMAGE
	cam.release();
	return 0;
}




