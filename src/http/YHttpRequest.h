#pragma once
#include "http/YHttpHeader.h"
#include "http/YHttpRespond.h"
#include "YTCPClient.h"

typedef std::function<void(YHttpRequest*, YHttpRespond*, const char*, int)> HttpRecvEventCallBack;
typedef std::function<void(YHttpRequest*)> HTTPEventCallBack;

//如果http通信需要比较长的数据的时候，比如，需要上传一个大文件。请使用这个
class YHttpAsyncData
{
public:
	//数据长度
	virtual int lenth() = 0;
	virtual std::vector<char> data(int begin, int end) = 0;
};

class YHttpRequest
{
public:
	YHttpHeader& getHeader();
	void request(const std::string &method, const std::string &url, std::vector<char>* data = nullptr);
	//当需要提交较大的数据量的时候，建议使用这个
	void request(const std::string &method, const std::string &url, YHttpAsyncData* data);
	void setIOPoll(IOPoll*);
	void onClose(HTTPEventCallBack);
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
	YHttpAsyncData* mYHttpAsyncData;
	std::vector<char> mRespondHeaderBuff;
	
	YHttpRespond mYHttpRespond;

	YTCPClient mClient;

	HttpRecvEventCallBack mReadCallBack;
	HTTPEventCallBack mCloseCallBack;
};