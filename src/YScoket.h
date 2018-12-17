#pragma once
#include "socket_header.h"
#include "poll/IOPoll.h"

#ifndef Y_BUFF_SIZE 
#define Y_BUFF_SIZE 1024 
#endif

class YAddress;
class YSocket;

typedef std::function<void(YSocket*)> SocketEventCallBack;

class YSocket
{
public:
	YSocket();
	YSocket(SOCKET_FD fd);
	virtual ~YSocket();
	void bind(YAddress* address, bool allow_reuse = true);

	virtual bool close();
	bool isClosed();

	bool isBlocking();
	void setBlocking(bool blocking);
	
	unsigned int getTimeout();
	void setTimeout(unsigned int timeout);

	bool getOption(int level, int optname, void *optval, socklen_t *optlen);
	bool setOption(int level, int optname, const void *optval, socklen_t optlen);
	
	int getLastError();
	SOCKET_FD fd();
	//YAddress* localAddress();
	virtual bool isTCP() = 0;

public:
	virtual void setIOPoll(IOPoll*);

	void onError(SocketEventCallBack);
	void onWrite(SocketEventCallBack);
	
protected:
	virtual void onErrorCallBack();
	virtual void onWriteCallBack();
	virtual void onReadCallBack();

	SocketEventCallBack  errorCallBack;
	SocketEventCallBack  writeCallBack;
	SocketEventCallBack  readCallBack;
	IOPoll* mPoll;
protected:
	void createSocketIfNecessary(bool isIPV6);
private:
	bool isBlock;
	bool mIsClose;
	SOCKET_FD mfd;
};

