#include "YTCPClient.h"
#include "YAddress.h"
#include "ssl/YTLS.h"

bool YTCPClient::connect(YAddress* address)
{
	createSocketIfNecessary(address->isIPV6());
	setRemoteAddress(address->getSocketAddr());
	size_t len = 0;
	if (address->isIPV6())
	{
		len = sizeof(struct sockaddr_in6);
	}
	else
	{
		len = sizeof(struct sockaddr_in);
	}
	auto ret = ::connect(fd(), (struct sockaddr*)(address->getSocketAddr()), len) == 0;
	if (ret && mIsEableSSL)
	{
		return mYTLS->handshake();
	}
	return ret;
}

bool YTCPClient::isConnected()
{
	return mIsConnect;
}

int  YTCPClient::receive(char *buffer, size_t size)
{
	if (mIsEableSSL)
	{
		return mYTLS->receive(buffer, size);
	}
	else
	{
		return ::recv(fd(), buffer, size, 0);
	}	
}

int  YTCPClient::send(const char *buffer, size_t size)
{
	auto sendNum = 0;
	while (true)
	{
		int num = 0;
		
		if (mIsEableSSL)
		{
			num = mYTLS->send(buffer + sendNum, size - sendNum);
		}
		else
		{
			num = ::send(fd(), buffer + sendNum, size - sendNum, 0);
		}
		
		if (num <= 0)
		{
			break;
		}
		sendNum += num;
		if (sendNum == size)
		{
			break;
		}
	}
	return sendNum;
}

YAddress* YTCPClient::remote_address()
{
	return mRemoteAddress;
}

void YTCPClient::setRemoteAddress(void* addr)
{
	if (mRemoteAddress == NULL)
	{
		mRemoteAddress = new  YAddress(addr);
	}
	else
	{
		mRemoteAddress->reset(addr);
	}
}

void YTCPClient::onRead(TCPClientEventCallBack callBack) 
{
	mReadCallBack = callBack;
}

void YTCPClient::onClose(SocketEventCallBack callBack)
{
	mCloseCallBack = callBack;
}

void YTCPClient::onConnect(SocketEventCallBack callBack)
{
	mConnectCallBack = callBack;

}

void YTCPClient::onErrorCallBack()
{
	YSocket::onErrorCallBack();
	mIsConnect = false;
}
 
void YTCPClient::onWriteCallBack()
{
	if (!mIsConnect)
	{
		mIsConnect = true;
		if (mConnectCallBack)
		{
			mConnectCallBack(this);
		}
	}
	else {
		YSocket::onWriteCallBack();
	}
}
 void YTCPClient::onReadCallBack()
{
	 char* buff = mBuff;
	 int c = receive(buff, Y_BUFF_SIZE);
	 //此时服务器close了。
	 if (c == 0)
	 {
		 this->close();
		 if (mCloseCallBack) { mCloseCallBack(this); }
		 return;
	 }
	 if (c < 0)
	 {
		 this->close();
		 if (mCloseCallBack) { mCloseCallBack(this); }
		 return;
	 }

	 if (mReadCallBack)
	 {
		 mReadCallBack(this, buff, c);
	 }
	
}