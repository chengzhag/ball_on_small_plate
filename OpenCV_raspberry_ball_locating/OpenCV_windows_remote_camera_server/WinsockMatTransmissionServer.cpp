/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  基于OpenCV和Winsock的图像传输（接收）
//
//  By 彭曾 , at CUST, 2016.08.06
//
//  website: www.pengz0807.com  email: pengz0807@163.com
//
//M*/


#include "WinsockMatTransmissionServer.h"  
#include "iostream"

using namespace std;
using namespace cv;

WinsockMatTransmissionServer::WinsockMatTransmissionServer(void)
{
}


WinsockMatTransmissionServer::~WinsockMatTransmissionServer(void)
{
}


int WinsockMatTransmissionServer::begin(int port)
{
	printf("listening to port %d\n", port);
	if (socketConnect(port) < 0)
	{
		cout << "socket connecting error!" << endl;
		return -1;
	}
	cout << "socket connecting success!" << endl;
	return 1;
}

int WinsockMatTransmissionServer::socketConnect(int port)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return -1;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);
	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	listen(sockSrv, 5);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);

	int nRecvBuf = 1024 * 1024 * 10;
	setsockopt(sockConn, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
}


void WinsockMatTransmissionServer::disconnect(void)
{
	closesocket(sockConn);
}

int WinsockMatTransmissionServer::receive(cv::Mat& image, int flag)
{
	uint64_t bufSize = 0;
	if (recv(sockConn, (char*)&bufSize, sizeof(bufSize), MSG_WAITALL) != sizeof(bufSize))
	{
		printf("Server Recieve Head Failed!\n");
		return -1;
	}
	if (bufSize==0)
	{
		printf("Server Recieve Head 0!\n");
		return -1;
	}
	buf.resize(bufSize);

	//方案一，循环读取
	uint64_t index = 0, len = 0;
	while (index < bufSize)
	{
		len = recv(sockConn, (char*)buf.data() + index, bufSize - index, MSG_WAITALL);
		if (len < 0)
		{
			printf("Server Recieve Data Failed!\n");
			return -1;
		}
		index += len;
	}
	
	image = imdecode(buf, flag);
	return 1;
}