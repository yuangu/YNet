#pragma once 

#include <string>
#include <vector>

enum YDNSType
{
	DNSType_A = 1,  //��������ȡIpv4��ַ
	DNSType_NS = 2,    //��ѯ����������
	DNSType_CNAME = 5,   //��ѯ�淶����
	DNSType_SOA = 6,   //��ʼ��Ȩ
	DNSType_WKS = 11,   //��֪����
	DNSType_PTR = 12,   //��ip��ַת��������
	DNSType_HINFO = 13,  //������Ϣ
	DNSType_MX = 15,   //�ʼ�����
	DNSType_AAA = 28, //���������ipv6��ַ
	DNSType_AXFR = 252, //����������������
	DNSType_ANY = 255, //���м�¼������
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