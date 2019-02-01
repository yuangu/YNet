#include <stdio.h>
#include "YDNSResolver.h"
#include "ssl/YMbedtls.h"
#include "YTCPClient.h"
#include "YAddress.h"
#include "poll/SelectPoll.h"

#include "http/YHttpRequest.h"

int main() {
	YHttpRequest request;
	request.request("GET", "https://14.215.177.39");
	
	//SelectPoll selectPoll;
	//request.setIOPoll((IOPoll*)&selectPoll);
	request.onRead([](YHttpRequest*, YHttpRespond*, const char* buff, int dataLen) {
		auto f = fopen("test.html", "a+");
		fwrite((void*)buff, 1, dataLen, f);
		fclose(f);
	});

	request.perform();
	while (true)
	{
		Sleep(1);

	}


	/*while (1)
	{
		selectPoll.wait(1000);

	}*/


	/*YTCPClient tcpClient;
	YAddress address(std::string("14.215.177.39"), 443);
	tcpClient.setSSLFlag(true);
	tcpClient.connect(&address);
	
	const char* data = "GET / HTTP/1.1\r\n\r\n";
	int sendLen = tcpClient.send(data, strlen(data));
	char buff[1024] = {0x00};
	tcpClient.receive(buff, 1024);*/

	/*YDNSResolver resolver;
	std::vector<std::string> ret;
	resolver.lookupByName(std::string("www.google.com"), ret);*/
	return 0;
}