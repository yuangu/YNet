#pragma once

#include <string>
class YSocket;

class YTLS
{
public:

	virtual bool handshake() = 0; 
	virtual std::string version() = 0;
	virtual int  send(const char *buffer, size_t size) = 0;
	virtual int  receive(char *buffer, size_t size) = 0;
public:
	virtual int init(YSocket*) { return 0; };
	virtual void clean() {};
};