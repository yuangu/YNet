#pragma once
#include "IOPoll.h"



class SelectPoll :
	public IOPoll 
{
public:
	SelectPoll();
	bool wait(unsigned int  wait_ms);

	virtual  void registerFd(SOCKET_FD fd, PollEvent type, PollEventCallBack pollItem);
	virtual  void unRegisterFd(SOCKET_FD fd, PollEvent type = PollEventType::ALL);
private:
	fd_set          read_fds_;
	fd_set          write_fds_;
	fd_set          except_fds_;
	SOCKET_FD       max_fd_;
};