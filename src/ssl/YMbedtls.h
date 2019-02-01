#pragma once
#include "ssl/YTLS.h"

struct mbedtls_context;
class YSocket;
class YMbedtls:
	public YTLS
{
public:

	virtual bool handshake();
	virtual int  send(const char *buffer, size_t size);
	virtual int  receive(char *buffer, size_t size);
	virtual std::string version();

	virtual int init(YSocket*);
	virtual void clean();
private:
	mbedtls_context* mContext;
	YSocket* mSocket;
};