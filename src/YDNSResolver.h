#pragma once 

#include <string>
#include <vector>

enum YDNSType
{
	DNSType_A = 1,  //由域名获取Ipv4地址
	DNSType_NS = 2,    //查询域名服务器
	DNSType_CNAME = 5,   //查询规范名称
	DNSType_SOA = 6,   //开始授权
	DNSType_WKS = 11,   //熟知服务
	DNSType_PTR = 12,   //把ip地址转换成域名
	DNSType_HINFO = 13,  //主机信息
	DNSType_MX = 15,   //邮件交换
	DNSType_AAA = 28, //由域名获得ipv6地址
	DNSType_AXFR = 252, //传送整个区的请求
	DNSType_ANY = 255, //对有记录的请求
};

class YDNSResolver
{
public:
	bool lookupByName(std::string& host, std::vector<std::string>& ret, YDNSType type = DNSType_A, const char* DNSServer = "114.114.114.114");
private:
	void request(std::string& host, YDNSType type = DNSType_A, const char* DNSServer = "114.114.114.114");
	void onRespond(const char* _buff);

	void changetoDnsNameFormat(char *dns, const char *host);
	std::string readName(char* reader, int off, int& readLeng);
private:
	std::string mDNSServer;	
};