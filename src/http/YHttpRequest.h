#pragma once
#include "http/YHttpHeader.h"
#include "http/YHttpRespond.h"
#include "YTCPClient.h"

typedef std::function<void(YHttpRequest*, YHttpRespond*, const char*, int)> HttpRecvEventCallBack;


//���httpͨ����Ҫ�Ƚϳ������ݵ�ʱ�򣬱��磬��Ҫ�ϴ�һ�����ļ�����ʹ�����
class YHttpAsyncData
{
public:
	//���ݳ���
	virtual int lenth() = 0;
	virtual std::vector<char> data(int begin, int end) = 0;
};

class YHttpRequest
{
public:
	YHttpHeader& getHeader();
	void request(const std::string &method, const std::string &url, std::vector<char>* data = nullptr);
	void setIOPoll(IOPoll*);
	void onRead(HttpRecvEventCallBack);

private:
	void onTCPRead(YTCPClient*, const char*, int);
	void onTCPClose(YSocket*);
	void onTCPConnect(YSocket*);

	void reset();
private:
	std::vector<char> mWillSendData;
	YHttpHeader mHeader;
	YHttpHeader mRespondHeader;
	
	std::vector<char> mRespondHeaderBuff;
	
	YHttpRespond mYHttpRespond;

	YTCPClient mClient;

	HttpRecvEventCallBack mReadCallBack;
};