#include "http/YHttpRequest.h"
#include <stdlib.h>
#include <stdio.h>
#include "http/YURL.h"
#include "YAddress.h"

#include "YStrHelper.h"

YHttpHeader& YHttpRequest::getHeader()
{
	return mHeader;
}

void YHttpRequest::request(const std::string &method, const std::string &url, YHttpAsyncData* data)
{
	mHeader.addHeader(ContentLength, data->lenth());
	mYHttpAsyncData = data;
	request(method, url);
}

//请求
void YHttpRequest::request(const std::string &method, const std::string &url, std::vector<char>* data)
{
	reset();
	YURL yURL(url);
	

	//添加host头
	std::string host = yURL.getHost();
	if (yURL.getPort().length() > 0)
	{
		host.append(":");
		host.append(yURL.getPort());
	}
	mHeader.addHeader(HostHeaderName, host);

	std::string headUrl = yURL.getPath();

	if (yURL.getQuery().length() > 0)
	{
		headUrl.append("?");
		headUrl.append(yURL.getQuery());
	}
	
	unsigned short port = 80;


	if (yURL.getPort().length() > 0)
	{
		port = atoi(yURL.getPort().c_str());
	}
	else if (YStrHelper::isEqual(yURL.getScheme(), "https"))
	{
		port = 443;
	}

	std::string headerStr = mHeader.buildHeader(method, headUrl, VersionHTTP1_1);
	mWillSendData.clear();
	mWillSendData.insert(mWillSendData.end(), headerStr.begin(), headerStr.end());
	if (data)
	{
		mWillSendData.insert(mWillSendData.end(), data->begin(), data->end());
	}
	
	std::string serverHost = yURL.getHost();
	YAddress address(serverHost, port);
	mClient.onRead(std::bind(&YHttpRequest::onTCPRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mClient.onConnect(std::bind(&YHttpRequest::onTCPConnect, this, std::placeholders::_1));
	mClient.onClose(std::bind(&YHttpRequest::onTCPClose, this, std::placeholders::_1));

	//连接服务器
	bool isEnableSll = YStrHelper::isEqual(yURL.getScheme(), "https") ? true : false;
	mClient.setSSLFlag(isEnableSll);
	mClient.connect(&address);
}

void YHttpRequest::onTCPRead(YTCPClient* client, const char* buff, int dataLen)
{
	if (mYHttpRespond.mIsFinishRecvHeader)
	{
		if (mReadCallBack)
		{
			mReadCallBack(this, &mYHttpRespond, buff, dataLen);
		}
	}
	else
	{
		int dealNum = mYHttpRespond.dealHeader(buff, dataLen);
		if (dataLen - dealNum > 0)
		{
			if (mReadCallBack)
			{
				mReadCallBack(this, &mYHttpRespond, buff + dealNum, dataLen - dealNum);
			}
		}
	}
}

void YHttpRequest::onTCPClose(YSocket*)
{
	if (mCloseCallBack)
	{
		mCloseCallBack(this);
	}
}

void YHttpRequest::onTCPConnect(YSocket*)
{
	mClient.send(&(mWillSendData[0]), mWillSendData.size());
	if (!mYHttpAsyncData) return;

	int begin = 0;

	while (true)
	{
		int end = begin + 1024;
		if (end > mYHttpAsyncData->lenth())
		{
			end = mYHttpAsyncData->lenth();
		}
		mYHttpAsyncData->data(begin, end);
		begin = end + 1;
		if (begin > mYHttpAsyncData->lenth())
		{
			break;
		}
	}
}

void YHttpRequest::setIOPoll(IOPoll* poll)
{
	mClient.setIOPoll(poll);
}

void YHttpRequest::reset()
{
	mYHttpAsyncData = nullptr;
	mYHttpRespond.rest();
}


void YHttpRequest::onRead(HttpRecvEventCallBack callBack)
{
	mReadCallBack = callBack;
}

void YHttpRequest::onClose(HTTPEventCallBack  callBack)
{
	mCloseCallBack = callBack;
}

void YHttpRequest::perform()
{
	onTCPConnect(&mClient);
	char buff[1024] = {0x00};
	auto size = 0;
	
	while (true)
	{
		size = mClient.receive(buff, 1024);
		if (size <= 0) break;
		onTCPRead(&mClient, buff, size);
	}
	
}