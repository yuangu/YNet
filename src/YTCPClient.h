#pragma once

#include "YScoket.h"

class YTCPClient;
typedef std::function<void(YTCPClient*, const char*, int)> TCPClientEventCallBack;

class YTCPClient
	:public YSocket
{
public:
	YTCPClient() :
		YSocket() 
	{
		mIsConnect = false;
		mRemoteAddress = NULL;
	};

	YTCPClient(SOCKET_FD fd) :
		YSocket(fd)
	{
		mIsConnect = false;
		mRemoteAddress = NULL;
	};

	bool connect(YAddress* address);
	bool isConnected();
	int  receive(char *buffer, size_t size);
	int  send(const char *buffer, size_t size);
	YAddress* remote_address();
	void setRemoteAddress(void*);
	bool isTCP() { return true; };
	
public:
	void onRead(TCPClientEventCallBack callBack);
	void onClose(SocketEventCallBack callBack);
	void onConnect(SocketEventCallBack callBack);
private:
	virtual void onErrorCallBack();
	virtual void onWriteCallBack();
	virtual void onReadCallBack();

	TCPClientEventCallBack mReadCallBack;
	SocketEventCallBack mCloseCallBack;
	SocketEventCallBack mConnectCallBack;

private:
	YAddress* mRemoteAddress;
	char mBuff[Y_BUFF_SIZE];
	bool mIsConnect;

};


