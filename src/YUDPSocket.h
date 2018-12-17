#pragma once

#include "YScoket.h"

class YUDPSocket;
typedef std::function<void(YUDPSocket*, YAddress*, const char*, int)> UDPSocketEventCallBack;
class YUDPSocket
	:public YSocket
{
public:
	YUDPSocket(bool isIVP6 = false)
		:YSocket() {
		this->createSocketIfNecessary(isIVP6);	
	};
	int  receiveFrom(YAddress* address, char *buffer, size_t size);
	int  sendTo(YAddress* address, char *buffer, size_t size);
	bool isTCP() { return false; };

	bool canBroadcast();
	void setBroadcast(bool broadcast);
	void onRead(UDPSocketEventCallBack callBack);
private:
	virtual void onReadCallBack();
private:
	UDPSocketEventCallBack mReadCallBack;


};


