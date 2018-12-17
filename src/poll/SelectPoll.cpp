#include "SelectPoll.h"
#include <vector>

bool SelectPoll::wait(unsigned int  wait_ms)
{
	fd_set readfds, writefds, exceptfds;
	memcpy(&readfds, &read_fds_, sizeof(read_fds_));
	memcpy(&writefds, &write_fds_, sizeof(write_fds_));
	memcpy(&exceptfds, &except_fds_, sizeof(except_fds_));
	struct timeval tval { 0, 0 };
	if (wait_ms != -1) {
		tval.tv_sec = wait_ms / 1000;
		tval.tv_usec = (wait_ms - tval.tv_sec * 1000) * 1000;
	}
	int nready = ::select(max_fd_ + 1, &readfds, &writefds, &exceptfds, wait_ms == -1 ? NULL : &tval);
	if (nready <= 0) {
		return false;
	}

	std::vector<SOCKET_FD> fd_list;
	
	for (auto it = mPollItemMap->begin(); it != mPollItemMap->end(); ++it)
	{
		auto fd = it->first;
		fd_list.push_back(fd);
	}

	for(auto it_l = fd_list.begin();  it_l != fd_list.end(); ++it_l){
		auto it = mPollItemMap->find(*it_l);
		if (it == mPollItemMap->end())
		{
			continue;
		}

		auto fd = it->first;
		auto item = it->second;
		if (FD_ISSET(fd, &readfds) && item->readCallBack) {
			item->readCallBack();
		}
		if ( FD_ISSET(fd, &writefds) && item->writeCallBack) {
			item->writeCallBack();
		}
		if (FD_ISSET(fd, &exceptfds) && item->errorCallBack) {
			item->errorCallBack();
		}
	}
}

SelectPoll::SelectPoll()
{
	max_fd_ = INVALID_FD;
	FD_ZERO(&read_fds_);
	FD_ZERO(&write_fds_);
	FD_ZERO(&except_fds_);
}

void SelectPoll::registerFd(SOCKET_FD fd, PollEvent type, PollEventCallBack pollItem)
{
	IOPoll::registerFd(fd, type, pollItem);
	if (max_fd_  == INVALID_FD || fd > max_fd_)
	{
		max_fd_ = fd;
	}

	if (type & PollEventType::Write)
	{
		FD_SET(fd, &write_fds_);
		
	}

	if (type & PollEventType::Read)
	{
		FD_SET(fd, &read_fds_);
	}

	if (type & PollEventType::Error)
	{
		FD_SET(fd, &except_fds_);
	}

}

void SelectPoll::unRegisterFd(SOCKET_FD fd, PollEvent type )
{
	IOPoll::unRegisterFd(fd, type);
	if (fd == max_fd_)
	{
		auto it = mPollItemMap->find(fd);
		if (it != mPollItemMap->end())
		{
			return;
		}
		//计算出新的最大的
		max_fd_ = INVALID_FD;
		for (auto it = mPollItemMap->begin(); it != mPollItemMap->end(); ++it)
		{
			auto fd = it->first;

			if (max_fd_ == INVALID_FD ||  fd > max_fd_)
			{
				max_fd_ = fd;
			}
		}
	}

	if (type & PollEventType::Write)
	{
		FD_CLR(fd, &write_fds_);

	}

	if (type & PollEventType::Read)
	{
		FD_CLR(fd, &read_fds_);
	}

	if (type & PollEventType::Error)
	{
		FD_CLR(fd, &except_fds_);
	}
	
}