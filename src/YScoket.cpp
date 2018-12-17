#include "YScoket.h"
#include <assert.h>
#include "YAddress.h"

#if YG_PLATFORM == YG_PLATFORM_WIN32
struct __WSAStartup__AutoIniter__
{
	__WSAStartup__AutoIniter__()
	{
		WSADATA wsaData;
		assert(WSAStartup(MAKEWORD(2, 2), &wsaData)==0);
	};

	~__WSAStartup__AutoIniter__()
	{
		WSACleanup();
	}
} __WSAStartup__AutoIniter__;
#endif

YSocket::YSocket()
{
	mfd = INVALID_FD;
}

YSocket::YSocket(SOCKET_FD fd)
{
	assert(fd != INVALID_FD);
	mfd = fd;
}

void YSocket::createSocketIfNecessary(bool isIPV6)
{
	assert(mfd == INVALID_FD);
	
	short family = AF_INET;
	if (isIPV6)
	{
		family = AF_INET6;
	}

	int type = SOCK_DGRAM;
	if (isTCP())
	{
		type = SOCK_STREAM;
	}

	mfd = socket(family, type, 0);
	setBlocking(true);
}

void YSocket::bind(YAddress* address, bool allow_reuse)
{
	createSocketIfNecessary(address->isIPV6());
	size_t len = 0;
	if (address->isIPV6())
	{
		len = sizeof(struct sockaddr_in6);
	}
	else
	{
		len = sizeof(struct sockaddr_in);
	}
	::bind(mfd, (struct sockaddr*)(address->getSocketAddr()), len);

	if (allow_reuse)
	{
		int  optval = 1;
		setOption(SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	}
	else
	{
		int  optval = 0;
		setOption(SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	}
}

bool  YSocket::getOption(int level, int optname,  void *optval, socklen_t *optlen)
{
	assert(mfd != INVALID_FD);
	return getsockopt(mfd, level, optname, (char*)optval, optlen) >= 0;
}

bool  YSocket::setOption(int level, int optname, const void *optval, socklen_t optlen)
{
	assert(mfd != INVALID_FD);
	return setsockopt(mfd, level, optname, (const char*)optval, optlen) >= 0;
}

bool YSocket::isBlocking()
{
	assert(mfd != INVALID_FD);
	return isBlock;
}

void YSocket::setBlocking(bool blocking)
{
	assert(mfd != INVALID_FD);
	isBlock = blocking;
#if YG_PLATFORM == YG_PLATFORM_WIN32
//https://blog.csdn.net/Timmy_zhou/article/details/6029470
	if (blocking)
	{
		static const int block = 0;
		ioctlsocket(mfd, FIONBIO, (u_long*)&block);
	}
	else 
	{
		static const int noblock = 1;
		ioctlsocket(mfd, FIONBIO, (u_long*)&noblock);
	}
	
#else
//https://blog.csdn.net/u010871058/article/details/76147082
	int flags = fcntl(mfd, F_GETFL, 0);
	if (blocking) 
	{
		flags = flags & ~O_NONBLOCK;
		ret = fcntl(mfd, F_SETFL, flags);
	}else
	{
		flags = flags | O_NONBLOCK;
		ret = fcntl(mfd, F_SETFL, flags);
	}
#endif
}


SOCKET_FD YSocket::fd()
{
	return mfd;
}

bool YSocket::close()
{
	setIOPoll(nullptr);

	if (isClosed())
	{
		return true;
	}
	mIsClose = true;
#if YG_PLATFORM == YG_PLATFORM_WIN32
	return closesocket(mfd) == 0;
#else
	return close(mfd) == 0;
#endif
}

bool YSocket::isClosed()
{
	return mIsClose;
}

unsigned int YSocket::getTimeout()
{
	int len;
#if YG_PLATFORM == YG_PLATFORM_WIN32
	unsigned long  recvtimeout;
	getOption(SOL_SOCKET, SO_RCVTIMEO, &recvtimeout, &len);
	return recvtimeout / 1000;
#else
	unsigned int recvtimeout;
	getOption(SOL_SOCKET, SO_RCVTIMEO,&recvtimeout, &len);
	return recvtimeout;
#endif
}

void YSocket::setTimeout(unsigned int timeout)
{
#if YG_PLATFORM == YG_PLATFORM_WIN32
	unsigned long  recvtimeout = timeout * 1000;
	setOption(SOL_SOCKET, SO_RCVTIMEO,
		(const char *)&recvtimeout, sizeof(recvtimeout));
#else
	setOption(SOL_SOCKET, SO_RCVTIMEO,
	(const char *)&timeout, sizeof(timeout));
#endif
}

int YSocket::getLastError()
{
#if YG_PLATFORM == YG_PLATFORM_WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void YSocket::setIOPoll(IOPoll* poll)
{
	if (poll == nullptr)
	{
		if (mPoll != nullptr)
		{
			mPoll->unRegisterFd(fd());
		}
		
		mPoll = poll;
		return;
	}
	mPoll = poll;

	poll->registerFd(fd(), PollEventType::Write, std::bind(&YSocket::onWriteCallBack, this));
	poll->registerFd(fd(), PollEventType::Read, std::bind(&YSocket::onReadCallBack, this));
	poll->registerFd(fd(), PollEventType::Error, std::bind(&YSocket::onErrorCallBack, this));
		
	setBlocking(false);
}

void YSocket::onError(SocketEventCallBack callBack)
{
	errorCallBack = callBack;
}

void YSocket::onWrite(SocketEventCallBack callBack)
{
	writeCallBack = callBack;	
}

void YSocket::onErrorCallBack()
{
	if (errorCallBack)
	{
		errorCallBack(this);
	}
	setIOPoll(nullptr);
}

void YSocket::onWriteCallBack()
{
	if (writeCallBack)
	{
		writeCallBack(this);
	}
}

void YSocket::onReadCallBack()
{
	if (readCallBack)
	{
		readCallBack(this);
	}
}

YSocket::~YSocket()
{
	if (mPoll)
	{
		mPoll->unRegisterFd(fd());
	}
}