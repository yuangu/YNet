#pragma once
#include "ssl/YTLS.h"

class YMbedtls:
	public YTLS
{
public:
	virtual int init() = 0;
	virtual void clean() = 0;
	virtual std::string version() = 0;
};