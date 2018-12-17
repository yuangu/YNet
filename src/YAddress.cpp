#include "YAddress.h"

YAddress::YAddress(std::string& host, unsigned short port)
{
	reset(host, port);
}

YAddress::YAddress(void* address) 
{
	reset(address);
}

void YAddress::reset(void* address)
{
	memset(&mss, 0, sizeof(struct sockaddr_storage));
	sockaddr* ss = reinterpret_cast<struct sockaddr*>(address);
	if (ss->sa_family == AF_INET6)
	{
		memcpy(&mss, address, sizeof(struct sockaddr_in6));
	}
	else if (ss->sa_family == AF_INET)
	{
		memcpy(&mss, address, sizeof(struct sockaddr_in));
	}
	resetHostAndPort();
}

void YAddress::reset(std::string& host, unsigned short port)
{
	bool _isIPV6 = false;
	//ipv6
	if (host.find(':') != std::string::npos)
	{
		_isIPV6 = true;
	}
	memset(&mss, 0, sizeof(struct sockaddr_storage));

	if (_isIPV6)
	{
		struct sockaddr_in6 *addr_v6 = (struct sockaddr_in6 *)&mss;
		addr_v6->sin6_family = AF_INET6;
		addr_v6->sin6_port = htons(port);
		inet_pton(AF_INET6, host.c_str(), &(addr_v6->sin6_addr));
	}
	else {
		struct sockaddr_in *addr_v4 = (struct sockaddr_in *)&mss;
		addr_v4->sin_family = AF_INET;
		addr_v4->sin_port = htons(port);
		inet_pton(AF_INET, host.c_str(), &(addr_v4->sin_addr));
	}
	resetHostAndPort();
}

bool YAddress::isIPV6()
{
	return mss.ss_family == AF_INET6;
}

void YAddress::resetHostAndPort()
{
	mHost.clear();
	if (isIPV6())
	{
		char buf[INET6_ADDRSTRLEN] = { 0x00 };
		struct sockaddr_in6* addr_v6 = reinterpret_cast<struct sockaddr_in6*>(&mss);
		inet_ntop(AF_INET6, &(addr_v6->sin6_addr), buf, INET6_ADDRSTRLEN);
		mHost.append(buf);
		mPort = ntohs(addr_v6->sin6_port);
	}
	else {
		struct sockaddr_in* addr_v4 = reinterpret_cast<struct sockaddr_in*>(&mss);
		char buf[INET_ADDRSTRLEN] = { 0x00 };
		inet_ntop(AF_INET, &(addr_v4->sin_addr), buf, INET_ADDRSTRLEN);
		mHost.append(buf);
		mPort = ntohs(addr_v4->sin_port);
	}
}

std::string YAddress::getHost() const
{
	return mHost;
}

unsigned short YAddress::getPort()
{
	return mPort;
}


struct sockaddr_storage* YAddress::getSocketAddr()
{
	return &mss;
}