#include "http/YHttpRespond.h"
#include "YStrHelper.h"

void YHttpRespond::rest()
{
	mHeadDealBuff.clear();
	mIsFinishRecvHeader = false;
	isDealStartLine = false;
}

int YHttpRespond::dealHeader(const char* buff, int dataLen)
{
	for (int i = 0; i < dataLen; ++i)
	{
		char byte = *(buff + i);
		if (byte == '\n' && mHeadDealBuff.size() > 0 && mHeadDealBuff[mHeadDealBuff.size() - 1] == '\r')
		{
			if (mHeadDealBuff.size() == 1)
			{
				mIsFinishRecvHeader = true;
				
				return i + 1;
			}

			//删除最后一个
			mHeadDealBuff.erase(mHeadDealBuff.end() - 1);
			if (isDealStartLine)
			{
				parseHeaderLine();
				
			}
			else
			{
				parseStartLine();
				isDealStartLine = true;
			}

			mHeadDealBuff.clear();
			continue;
		}
		mHeadDealBuff.push_back(byte);
	}
	return dataLen;
}


bool YHttpRespond::parseStartLine()
{
	std::string startLineDataList[3];
	int num = 0;
	int strLen = 0;
	
	for (auto it = mHeadDealBuff.begin(); it != mHeadDealBuff.end(); ++it)
	{
		if (*it == ' ')
		{
			startLineDataList[num].insert(startLineDataList[num].begin(), it - strLen, it);
			strLen = 0;
			num++;
		}
		else
		{
			++strLen;
		}
		
		if (num == 2)
		{
			startLineDataList[2].insert(startLineDataList[2].begin(), it, mHeadDealBuff.end());
			break;
		}
	}

	mStatusStr.clear();
	mStatusStr.append(startLineDataList[2].c_str(), startLineDataList[2].length());

	mStatusCode = std::atoi(startLineDataList[1].c_str());
	mHttpVersionStr.clear();
	mHttpVersionStr.append(startLineDataList[0].c_str(), startLineDataList[0].length());
	return true;
}


bool YHttpRespond::parseHeaderLine()
{
	std::string name;
	std::string vaule;
	int strLen = 0;
	for (auto it = mHeadDealBuff.begin(); it != mHeadDealBuff.end(); ++it)
	{
		if (*it == ':')
		{
			name.insert(name.end(), it - strLen, it);
			vaule.insert(vaule.end(), it + 1, mHeadDealBuff.end());
			YStrHelper::trim(vaule);
			break;
		}
		strLen++;
	}
	mHeader.addHeader(name, vaule);
	return true;
}