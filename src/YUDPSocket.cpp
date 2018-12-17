#include "YUDPSocket.h"
#include "YAddress.h"

int  YUDPSocket::receiveFrom(YAddress* address, char *buffer, size_t size)
{
	size_t len = 0;
	if (address->isIPV6())
	{
		len = sizeof(struct sockaddr_in6);
	}
	else
	{
		len = sizeof(struct sockaddr_in);
	}
	struct sockaddr_storage add;
	auto  ret = ::recvfrom(fd(), buffer, size, 0, (struct sockaddr*)&add, (int*)&len);
	address->reset(&add);
	return  ret;
}

int  YUDPSocket::sendTo(YAddress* address, char *buffer, size_t size)
{
	size_t len = 0;
	if (address->isIPV6())
	{
		len = sizeof(struct sockaddr_in6);
	}
	else
	{
		len = sizeof(struct sockaddr_in);
	}
	return ::sendto(fd(), buffer, size,0, (struct sockaddr*)(address->getSocketAddr()), len);
}

bool YUDPSocket::canBroadcast()
{
	bool broadcast;
	int len;
	getOption(SOL_SOCKET, SO_BROADCAST, &broadcast, &len);
	return broadcast;
}

void YUDPSocket::setBroadcast(bool broadcast)
{

	setOption(SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(bool));
}

void YUDPSocket::onRead(UDPSocketEventCallBack callBack)
{
	mReadCallBack = callBack;
}

void YUDPSocket::onReadCallBack()
{
	YAddress address;
	char buff[Y_BUFF_SIZE] = {0x00};
	auto t = receiveFrom(&address, buff, Y_BUFF_SIZE);
	mReadCallBack(this, &address, buff, t);
}