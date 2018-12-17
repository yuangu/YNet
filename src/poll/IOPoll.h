#pragma once 
#include <map>
#include <memory> 
#include <functional>
#include "macros.h"
#include "socket_header.h"

class YSocket;
typedef std::function<void()> PollEventCallBack;


namespace PollEventType
{
	extern unsigned int Read ;
	extern unsigned int Write;
	extern unsigned int Error;
	extern unsigned int ALL;
};


typedef  unsigned int  PollEvent;

struct PollItem 
{
	PollEventCallBack readCallBack;
	PollEventCallBack writeCallBack;
	PollEventCallBack errorCallBack;
};

typedef std::shared_ptr<PollItem> PollItemPtr;
typedef std::map<SOCKET_FD, PollItemPtr> PollItemMap;

class IOPoll
{
public:
	IOPoll(); 
	virtual ~IOPoll();
	virtual void registerFd(SOCKET_FD fd, PollEvent type, PollEventCallBack pollItem);
	virtual void unRegisterFd(SOCKET_FD fd, PollEvent type = PollEventType::ALL);

	virtual  bool wait(unsigned int  wait_ms) = 0;
protected:
	PollItemMap* mPollItemMap;
};





