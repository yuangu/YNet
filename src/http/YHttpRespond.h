#pragma once
#include "http/YHttpHeader.h"

class YHttpRequest;

class YHttpRespond
{
	friend class YHttpRequest;

public:
	void rest();
private:
	int  dealHeader(const char*, int);
	bool  parseStartLine();
	bool  parseHeaderLine();
private:
	std::string mHttpVersionStr;
	int  mStatusCode;
	std::string mStatusStr;

private:
	//没有处理完的数据 
	std::vector<char> mHeadDealBuff;
	bool mIsFinishRecvHeader;
	bool isDealStartLine;
	YHttpHeader mHeader;
};