#pragma once 

#include <string>

class YDNSResolver
{
public:
	bool lookupByName(std::string& host);
private:
	void ChangetoDnsNameFormat(char *dns, const char *host);
};