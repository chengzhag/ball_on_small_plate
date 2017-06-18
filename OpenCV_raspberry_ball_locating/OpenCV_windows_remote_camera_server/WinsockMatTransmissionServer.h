/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  基于OpenCV和Winsock的图像传输（接收）
//
//  By 彭曾 , at CUST, 2016.08.06
//
//  website: www.pengz0807.com  email: pengz0807@163.com
//
//M*/

#ifndef __WINSOCKMATTRANSMISSIONSEVER_H__  
#define __WINSOCKMATTRANSMISSIONSEVER_H__  

#include "opencv2/opencv.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/core/core.hpp"  
#include <stdio.h>  
#include <Winsock2.h>  

#pragma comment(lib,"WS2_32.lib")  



class WinsockMatTransmissionServer
{
	std::vector<uchar> buf;
	SOCKET sockConn;

	// 打开socket连接  
	// params : port    传输端口  
	// return : -1      连接失败  
	//          1       连接成功  
	int socketConnect(int port);

public:

	WinsockMatTransmissionServer(void);
	~WinsockMatTransmissionServer(void);

	//开始连接
	//params：port	监听端口
	//return：-1		连接失败
	//			1		连接成功
	int begin(int port=6666);


	// 接收图像  
	// params : image   待接收图像  
	// return : -1      接收失败  
	//          1       接收成功  
	int receive(cv::Mat& image, int flag = cv::IMREAD_GRAYSCALE);


	// 断开socket连接  
	void disconnect(void);
};

#endif  