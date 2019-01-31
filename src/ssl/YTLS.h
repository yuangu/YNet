#pragma once

#include <string>


class YTLS
{
public:
	virtual int init() = 0;
	virtual void clean() = 0;
	virtual std::string version() = 0;

};