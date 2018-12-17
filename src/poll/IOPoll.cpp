#include "IOPoll.h"


namespace PollEventType
{
	unsigned int Read = 1;
	unsigned int Write = (1 << 1);
	unsigned int Error = (1 << 2);
	unsigned int ALL = (PollEventType::Read | PollEventType::Write | PollEventType::Error);
};

IOPoll::IOPoll()
{
	mPollItemMap = new PollItemMap();
}

IOPoll::~IOPoll()
{

}

void IOPoll::registerFd(SOCKET_FD fd, PollEvent type, PollEventCallBack callBack)
{
	auto it = mPollItemMap->find(fd);
	if (it == mPollItemMap->end())
	{
		auto pollItem = new PollItem();
		mPollItemMap->insert(std::make_pair(fd, pollItem));
	}
	
	auto pollItem = (*mPollItemMap)[fd];

	if (type & PollEventType::Write)
	{
		pollItem->writeCallBack = callBack;
	}

	if (type & PollEventType::Read)
	{
		pollItem->readCallBack = callBack;
	}

	if (type & PollEventType::Error)
	{
		pollItem->errorCallBack = callBack;
	}
}

void IOPoll::unRegisterFd(SOCKET_FD fd, PollEvent type)
{
	auto it = mPollItemMap->find(fd);
	if (it == mPollItemMap->end())
	{
		
		return;
	}	

	if (type == PollEventType::ALL)
	{
		mPollItemMap->erase(it);
		return;
	}


	auto pollItem = (*mPollItemMap)[fd];

	if (type & PollEventType::Write)
	{
		pollItem->writeCallBack = nullptr;
	}

	if (type & PollEventType::Read)
	{
		pollItem->readCallBack = nullptr;
	}

	if (type & PollEventType::Error)
	{
		pollItem->errorCallBack = nullptr;
	}

}



