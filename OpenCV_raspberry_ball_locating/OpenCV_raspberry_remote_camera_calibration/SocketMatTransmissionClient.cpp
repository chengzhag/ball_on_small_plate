/*M/////////////////////////////////////////////////////////////////////////////////////// 
// 
//  基于OpenCV和Socket的图像传输（发送） 
//   
//  By 彭曾 , at CUST, 2016.08.07  
// 
//  website: www.pengz0807.com  email: pengz0807@163.com  
//   
//M*/  
  
  
#include "SocketMatTransmissionClient.h"  
#include <iostream>
#include "my_math.h"

using namespace std;
using namespace cv;  

SocketMatTransmissionClient::SocketMatTransmissionClient(void)  
{  
}  
  
  
SocketMatTransmissionClient::~SocketMatTransmissionClient(void)  
{  
}  
  
  
int SocketMatTransmissionClient::socketConnect(const char* ip, int port)  
{  
	struct sockaddr_in    servaddr;  
  
	if ((sockClient = socket(AF_INET, SOCK_STREAM, 0)) < 0)   
	{  
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}  
  
	memset(&servaddr, 0, sizeof(servaddr));  
	servaddr.sin_family = AF_INET;  
	servaddr.sin_port = htons(port);  
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)   
	{  
		printf("inet_pton error for %s\n", ip);  
		return -1;  
	}  
  
	if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)   
	{  
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}  
	else   
	{  
		printf("connect successful!\n");  
	}  
}  
  
int SocketMatTransmissionClient::begin(const char* ip, int port)
{
	printf("socket connecting to %s:%d\n", ip, port);
	if (socketConnect(ip, port) < 0)  
	{  
		cout << "socket connecting error!" << endl;
		return -1;  
	}  
	cout << "socket connecting success!" << endl;
	return 1;
}
  
void SocketMatTransmissionClient::disconnect(void)  
{  
	close(sockClient);  
}  
  
int SocketMatTransmissionClient::transmit(cv::Mat image, int quality)  
{  
	vector<int> compressionParams; 
	compressionParams.push_back(IMWRITE_JPEG_QUALITY);
	limit(quality, 0, 100);
	compressionParams.push_back(quality);
	imencode(".jpg",
		image,
		buf,
		compressionParams);
	uint64_t bufSize = buf.size();
	if (buf.data() == NULL)
	{
		printf("empty image\n\n");  
		return -1;  
	}
	if (send(sockClient, &bufSize, sizeof(bufSize), 0) < 0)
	{
		printf("send head error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}
	if (send(sockClient, buf.data(), bufSize, 0) < 0)  
	{  
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}  
	return 1;
	
}  