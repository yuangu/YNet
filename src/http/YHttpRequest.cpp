#include "http/YHttpRequest.h"
#include "http/YURL.h"
#include "YAddress.h"

YHttpHeader& YHttpRequest::getHeader()
{
	return mHeader;
}

//����
void YHttpRequest::request(const std::string &method, const std::string &url, std::vector<char>* data )
{
	reset();
	YURL yURL(url);
	yURL.getScheme();

	//���hostͷ
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
		port =  std::atoi(yURL.getPort().c_str());
	}

	std::string headerStr = mHeader.buildHeader(method, headUrl, VersionHTTP1_1);
	mWillSendData.clear();
	mWillSendData.insert(mWillSendData.end(), headerStr.begin(), headerStr.end());
	if (data)
	{
		mWillSendData.insert(mWillSendData.end(), data->begin(), data->end());
	}

	YAddress address((std::string)yURL.getHost(), port);
	mClient.onRead(std::bind(&YHttpRequest::onTCPRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mClient.onConnect(std::bind(&YHttpRequest::onTCPConnect, this,  std::placeholders::_1));
	mClient.onClose(std::bind(&YHttpRequest::onTCPClose, this, std::placeholders::_1));

	//���ӷ�����
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

}

void YHttpRequest::onTCPConnect(YSocket*)
{
	mClient.send(&(mWillSendData[0]), mWillSendData.size());
	
	char buff[1024];
	mClient.receive(buff, 1024);
}

void YHttpRequest::setIOPoll(IOPoll* poll)
{
	mClient.setIOPoll(poll);
}

void YHttpRequest::reset()
{
	mYHttpRespond.rest();
}


void YHttpRequest::onRead(HttpRecvEventCallBack callBack)
{
	mReadCallBack = callBack;
}