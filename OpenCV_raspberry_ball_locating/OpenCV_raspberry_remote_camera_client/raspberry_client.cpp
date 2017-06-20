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

#define STDIO_DEBUG
#define SOCKET_SEND_IMAGE


//void kmeans(const vector<Vec2f>& points, int K, vector<int>& labels, TermCriteria criteria, int attempts, int flags, vector<Vec2f>& centers)
//{
//
//}


int main(int argc, char **argv)
{
	///声明变量
	//数据采集
	raspicam::RaspiCam_Cv cam;
	Mat imRaw;
#ifdef SOCKET_SEND_IMAGE
	//发送图像，用于测试
	Mat imSend;
#endif // SOCKET_SEND_IMAGE
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, cam.get(CV_CAP_PROP_FRAME_WIDTH) * 0.2);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, cam.get(CV_CAP_PROP_FRAME_HEIGHT) * 0.2);
	const int imRawH = cam.get(CV_CAP_PROP_FRAME_HEIGHT),
		imRawW = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	
//	//算法相关
//	//剪切导轨位置图像
//	float railRegionHeight = 0.005, railRegionShift = -0.06;
//	Rect railRegion(0,
//		int(rawImHeight*(0.5 - railRegionHeight / 2 + railRegionShift)),
//		rawImWitdh,
//		rawImHeight*railRegionHeight);
//	//预处理
//	const int structElementSize = 3;
//	Mat element = getStructuringElement(MORPH_ELLIPSE,  
//		Size(2*structElementSize + 1, 2*structElementSize + 1),  
//		Point(structElementSize, structElementSize));
//	//将灰度投影到水平方向
//	vector<unsigned long> verticalVector;
//	//小球位置计算，单位mm
//	const float railLength=300;
//	const float camCenterShift = 0;
	
	//初始化连接
	if (!cam.open())
		return 1;

#ifdef SOCKET_SEND_IMAGE
	cout << "socket connecting..." << endl;
	SocketMatTransmissionClient socketMat;
	socketMat.begin("192.168.2.100");
#endif // SOCKET_SEND_IMAGE
	
	
//	//初始化串口
//	UartNum<float> uart;
//	uart.begin();

	double timeStart = 0, timeEnd = 0;
	while (1)
	{
		cam.grab();
		cam.retrieve(imRaw);

		if (imRaw.empty())
			return 1;
		
		/// 小球定位算法开始
		
		Mat imThresh;
//		threshold(imRaw, imThresh, 0, 255, CV_THRESH_OTSU);
//		threshold(imRaw, imThresh, 60, 255, CV_THRESH_BINARY_INV);
		Mat imEdge;
		const float resizeFactor = 0.3;
		resize(imRaw, imEdge, Size(0, 0), resizeFactor, resizeFactor, INTER_NEAREST);
		adaptiveThreshold(imEdge, imEdge, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, resizeFactor * 60);
//		Canny(imThresh, imEdge, 100, 200, 3);
		
		//矩形透视投影
		vector<Vec2f> lines;

		HoughLines(imEdge, lines, 1, CV_PI / 360 * 2, imRawH*resizeFactor * 0.3);
//		HoughLinesP(imEdge, lines, 1, CV_PI / 360 * 2, imRawH*resizeFactor * 0.3, imRawH*resizeFactor * 0.3, 3);
		for (size_t i = 0; i < lines.size(); i++) {
			float rho = lines[i][0]; 
			float theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta);
			double b = sin(theta);
			double x0 = rho*a;
			double y0 = rho*b;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*a);
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*a);
			line(imEdge, pt1, pt2, Scalar(125), 1, CV_AA);
		}
		
		int numPoints = lines.size();
		Mat points(numPoints, 2, CV_32F);
		Mat clusters(numPoints, 1, CV_32SC1);
		
		for (int k = 0; k < numPoints; k++)
		{
//			points.at(k, 0) = lines[k][0];
//			points.at(k, 1) = lines[k][1];
		}
//		kmeans(points, 4,)

		
		
		
		
//		cvtColor(rawIm, hsvIm, COLOR_BGR2HSV);
//		split(hsvIm, hsvSplit);  
//		equalizeHist(hsvSplit[2], hsvSplit[2]);  
//		merge(hsvSplit, hsvIm);

//		inRange(hsvIm,
//			Scalar(130, 0, 0),
//			Scalar(160, 255, 255),rawIm);
		
//		cvtColor(rawIm, rawIm, COLOR_HSV2BGR);
		
		
//		//剪切导轨位置图像
//		railIm = rawIm(railRegion);
//		
//		//预处理
//		morphologyEx(railIm, railIm, CV_MOP_ERODE, element);
//		GaussianBlur(railIm, railIm, Size(int(0.03*rawImWitdh) * 2 + 1, 1), int(0.02*rawImWitdh) * 2 + 1, 0);//以小球半径的两倍为窗口长度
////		medianBlur(railIm, railIm, 9);
////		morphologyEx(railIm, railIm, CV_MOP_DILATE, element);
////		medianBlur(railIm, railIm, 3);
////		equalizeHist(railIm, railIm);
////		threshold(railIm, railIm, 0, 255, CV_THRESH_OTSU);
//
//		//将灰度投影到水平方向
//		verticalProject(railIm, verticalVector);
//
//		//绘制亮度曲线图
////		Mat plotBrightness;
////		plotSimple(verticalVector, plotBrightness);
//		
//		//通过亮度曲线找到小球
//		vector<unsigned long>::iterator minBrightnessIt = min_element(verticalVector.begin(), verticalVector.end());
//		int minBrightnessPos = distance(verticalVector.begin(), minBrightnessIt);
//		
//		//直接以极小值为中心
//		float pos = (float(minBrightnessPos) - verticalVector.size() / 2)*railLength / verticalVector.size() - camCenterShift;
		
//		//以峰周围+-0.1*rawImWitdh的面积中心。弃用，波动更大
//		int peakStart = minBrightnessPos - 0.1*rawImWitdh, peakEnd;
//		limitLow<int>(peakStart, 0);
//		peakEnd = 2*minBrightnessPos - peakStart;
//		unsigned long peakArea = 0, indexWeighted = 0;
//		for (int i = peakStart; i < peakEnd; i++)
//		{
//			peakArea += verticalVector[i];
//			indexWeighted += verticalVector[i]*i;
//		}
//		float peakCenter = indexWeighted / peakArea;
//		float pos = (peakCenter - verticalVector.size() / 2)*railLength / verticalVector.size() + camCenterShift;
		
		
#ifdef STDIO_DEBUG
		//计算算法帧率
		cout << "fps: " << 1.0 / (timeEnd - timeStart)*(double)getTickFrequency() << endl;
		timeStart = timeEnd;
		timeEnd = (double)getTickCount();
#endif // STDIO_DEBUG
		
//		uart.sendNum(&pos, 1);

		///小球定位算法结束
		
#ifdef SOCKET_SEND_IMAGE
		//发送图像，用于测试
		resize(imEdge, imSend, Size(0, 0), 1, 1, INTER_NEAREST);
		socketMat.transmit(imSend, 90);
#endif // SOCKET_SEND_IMAGE
		
		
	}
#ifdef SOCKET_SEND_IMAGE
	socketMat.disconnect();
#endif // SOCKET_SEND_IMAGE
	cam.release();
	return 0;
}




