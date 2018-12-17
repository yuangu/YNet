#pragma once

#include <string>
#include "socket_header.h"


class YAddress
{
public:
	YAddress() {};
	YAddress(std::string& host, unsigned short port);
	YAddress(void* address);
	void reset(void* address);
	void reset(std::string& host, unsigned short port);
	bool isIPV6();
	std::string getHost() const;
	unsigned short getPort();
	struct sockaddr_storage* getSocketAddr();
private:
	void resetHostAndPort();
private:
	struct sockaddr_storage mss;

	std::string mHost;
	unsigned short mPort;
};
