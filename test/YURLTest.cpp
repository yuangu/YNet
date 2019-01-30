#include <gtest/gtest.h>

#include "poll/SelectPoll.h"
#include "http/YURL.h"
#include "http/YHttpHeader.h"
#include "http/YHttpRequest.h"


TEST(TestYURL, Nomal)
{
	YURL url;
	url.parse("http://www.huoyaojing.com");
	EXPECT_STREQ(url.getHost().c_str(), "www.huoyaojing.com");
}

TEST(TestYURL, GetQuery)
{
	YURL url;
	url.parse("http://www.huoyaojing.com/index.php?test=123");
	EXPECT_STREQ(url.getHost().c_str(), "www.huoyaojing.com");
}

TEST(TestHeader, Nomal)
{
	YHttpHeader header;
	header.addHeader(ContentType, "application/json");
	header.addHeader(ContentType, "application/json");
	EXPECT_EQ(header.getHeaders().size(), 1);
}


TEST(YHttpRequest, Nomal)
{
	YHttpRequest request;
	request.request("GET", "http://14.215.177.39");
	SelectPoll selectPoll;
	request.setIOPoll((IOPoll*)&selectPoll);
	request.onRead([](YHttpRequest*, YHttpRespond*, const char* buff, int dataLen) {
		auto f = fopen("test.html", "a+");
		fwrite((void*)buff,1, dataLen,f);
		fclose(f);
	});
	while (1)
	{
		selectPoll.wait(1000);

	}
}