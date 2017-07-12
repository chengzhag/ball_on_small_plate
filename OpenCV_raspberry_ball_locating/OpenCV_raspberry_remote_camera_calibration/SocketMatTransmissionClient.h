/*M/////////////////////////////////////////////////////////////////////////////////////// 
// 
//  基于OpenCV和Socket的图像传输（发送） 
//   
//  By 彭曾 , at CUST, 2016.08.07 
// 
//  website: www.pengz0807.com  email: pengz0807@163.com  
//   
//M*/  
  
#ifndef __SOCKETMATTRANSMISSIONCLIENT_H__  
#define __SOCKETMATTRANSMISSIONCLIENT_H__  
  
#include "opencv2/opencv.hpp"  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <vector>  
  

class SocketMatTransmissionClient  
{  
	std::vector<uchar> buf;
	int sockClient;  
	
	// 打开socket连接  
    // params : IP      服务器的ip地址  
    //          PORT    传输端口  
    // return : -1      连接失败  
    //          1       连接成功  
	int socketConnect(const char* ip, int port);  
	
public:  
	SocketMatTransmissionClient(void);  
	~SocketMatTransmissionClient(void);  

  
	//开始连接
	//params：port	监听端口
	//return：-1		连接失败
	//			1		连接成功
	int begin(const char* ip, int port = 6666);
  
  
    // 传输图像  
    // params : image 待传输图像  
    // return : -1      传输失败  
    //          1       传输成功  
	int transmit(cv::Mat image, int quality = 50);  
  
  
    // 断开socket连接  
	void disconnect(void);  
};  
  
#endif  