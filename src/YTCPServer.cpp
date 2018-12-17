#include "YTCPServer.h"
#include "YTCPClient.h"

YSocket* YTCPServer::accept()
{
	struct sockaddr_storage addr;
	socklen_t len =  sizeof(struct sockaddr_storage);
	auto newfd = ::accept(fd(), (sockaddr*)&addr, &len);
	YTCPClient* _YTCPServer = new YTCPClient((SOCKET_FD)newfd);
	_YTCPServer->setRemoteAddress((void*)&addr);
	return _YTCPServer;
}


void YTCPServer::listen()
{
	::listen(fd(), 5);
}

bool YTCPServer::isKeepalive()
{
	int optval = 0;
	int len;
	getOption(SOL_SOCKET, SO_KEEPALIVE, &optval, &len);
	return (optval == 0);
}

void YTCPServer::setKeepalive(bool keepAlive)
{
	int optval = 1;
	if (keepAlive)
	{
		optval = 0;
	}
	setOption(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
#ifdef TCP_KEEPCNT

	setOption(SOL_TCP, TCP_KEEPCNT, &optval, sizeof(optval));

#endif
#ifdef TCP_KEEPIDLE
	setOption(SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(optval));

#endif
#ifdef TCP_KEEPINTVL
	setOption(SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(optval));
#endif
}

void YTCPServer::onConnect(TCPServerEventCallBack callBack)
{
	mConnectCallBack = callBack;
}

void YTCPServer::onReadCallBack()
{
	YTCPClient* client = (YTCPClient*)(accept());
	mConnectCallBack(this, client);
}