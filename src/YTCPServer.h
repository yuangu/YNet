#pragma once

#include "YScoket.h"
class YTCPServer;
class YTCPClient;
typedef std::function<void(YTCPServer*,YTCPClient*)> TCPServerEventCallBack;
class YTCPServer
	:public YSocket
{
public:
	YTCPServer() :
		YSocket() {

	};

	YTCPServer(SOCKET_FD fd) :
		YSocket(fd) {

	};

	void listen();
	YSocket* accept();

	bool isKeepalive();
	void setKeepalive(bool keepAlive);
	bool isTCP() { return true; };
public:
	void onConnect(TCPServerEventCallBack callBack);
private:
	TCPServerEventCallBack mConnectCallBack;
	virtual void onReadCallBack();
private:
	YAddress* mRemoteAddress;
};


