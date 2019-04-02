#include "YDNSResolver.h"
#include "YAddress.h"
#include "YUDPSocket.h"
#include "YBinaryStream.h"

#define DNS_SERVER_PORT 53

//https://jocent.me/2017/06/18/dns-protocol-principle.html#_label1_0
struct DNS_HEADER {
	unsigned short id; // 会话标语，类似包序

	unsigned char rd : 1; // 表示期望递归
	unsigned char tc : 1; // 表示可截断的
	unsigned char aa : 1; // 表示授权回答
	unsigned char opcode : 4; // 0表示标准查询，1表示反向查询，2表示服务器状态请求
	unsigned char qr : 1; // 查询/响应标志，0为查询，1为响应

	unsigned char rcode : 4; // 表示返回码，0表示没有差错，3表示名字差错，2表示服务器错误（Server Failure）
	unsigned char cd : 1; // checking disabled
	unsigned char ad : 1; // authenticated data
	unsigned char z : 1; // its z! reserved
	unsigned char ra : 1; // 表示可用递归

	unsigned short q_count; // 表示查询问题区域节的数量
	unsigned short ans_count; //Answers表示回答区域的数量
	unsigned short auth_count; // 表示授权区域的数量
	unsigned short add_count; // 表示附加区域的数量
};

#pragma pack(push, 1)
struct QUESTION {
	unsigned short qtype; //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	unsigned short qclass; //通常为1，表明是Internet数据
};
#pragma pack(pop)

#pragma pack(push, 1)
struct R_DATA {
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD {
	std::string name;
	struct R_DATA  resource;
	std::vector<char> rdata;
};

bool YDNSResolver::lookupByName(std::string& host, std::vector<std::string>& ret, YDNSType type, const char* DNSServer)
{	
	size_t size = sizeof(DNS_HEADER) + sizeof(QUESTION) + host.length() + 2 ;
	char* buff = new char[size];
	struct DNS_HEADER *dns = (DNS_HEADER*) buff;

	dns->id = (unsigned short)100;  //会话id
	dns->qr = 0; //0表示请求 1表求响应
	dns->opcode = 0; //This is a standard query
	dns->aa = 0; //Not Authoritative
	dns->tc = 0; //This message is not truncated
	dns->rd = 1; //Recursion Desired
	dns->ra = 0; //Recursion not available! hey we dont have it (lol)
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1); //we have only 1 question
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;

	char* qname = buff + sizeof(struct DNS_HEADER);
	changetoDnsNameFormat(qname, host.c_str());
	struct QUESTION *qinfo = (struct QUESTION *) (buff + sizeof(DNS_HEADER) + host.length() + 2); //fill it

	qinfo->qtype = htons((short)type); //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	qinfo->qclass = htons(1); //通常为1，表明是Internet数据
	
	std::string dnsServerStdStr = std::string(DNSServer);
	YAddress address(dnsServerStdStr, (unsigned short)DNS_SERVER_PORT);
	YUDPSocket client(address.isIPV6());
	
	
	client.sendTo(&address, buff, size);

	char _buff[1024] = {0x00};
	client.setTimeout(1);
	int len = client.receiveFrom(&address, _buff, 1024);
	if (len < 0) return false;
	DNS_HEADER* header = (DNS_HEADER*)_buff;
	header->id;
	header->qr;

	char* tmp = _buff + 12;
	
	//请求区域
	for (int i = 0; i < ntohs( header->q_count); ++i)
	{
		int readLeng = 0;
		std::string name = readName(_buff, tmp - _buff, readLeng);
		tmp += readLeng;

		QUESTION* question = (QUESTION*)(tmp);
		question->qtype = ntohs(question->qtype);
		
		tmp += sizeof(QUESTION);
	}

	int ans_count = ntohs(header->ans_count);
	for (int i = 0; i < ans_count; ++i)
	{
		int readLeng = 0;
		std::string name = readName(_buff, tmp - _buff, readLeng);
		tmp += readLeng;

		R_DATA* rData = (R_DATA*)(tmp);
		rData->ttl = ntohl(rData->ttl);
		rData->type = ntohs(rData->type);
		rData->_class = ntohs(rData->_class);	
		rData->data_len = ntohs(rData->data_len);

		tmp += sizeof(R_DATA);
				
		
		switch (rData->type)
		{
		case DNSType_A:
		{
			char IPdotdec[INET_ADDRSTRLEN] = { 0x00 };
			inet_ntop(AF_INET, (void *)tmp, IPdotdec, INET_ADDRSTRLEN);
			std::string ipStr = IPdotdec;
			ret.push_back(ipStr);
			break;
		}
		case DNSType_AAA:
		{
			char IPdotdec[INET6_ADDRSTRLEN] = { 0x00 };
			inet_ntop(AF_INET6, (void *)tmp, IPdotdec, INET6_ADDRSTRLEN);
			std::string ipStr = IPdotdec;
			ret.push_back(ipStr);
		}
		default:
			break;
		}
		
		/*	std::string data;
		data.append(tmp + rData->data_len, ntohs(rData->data_len));*/
		tmp += rData->data_len;
	}
	
	for (int i = 0; i<ntohs(dns->add_count); i++)
	{
		int readLeng = 0;
		std::string name = readName(_buff, tmp - _buff, readLeng);
		tmp += readLeng;

		R_DATA* rData = (R_DATA*)(tmp);
		rData->ttl = ntohl(rData->ttl);
		rData->type = ntohs(rData->type);
		rData->_class = ntohs(rData->_class);
		rData->data_len = ntohs(rData->data_len);
		tmp += sizeof(R_DATA);

		/*	std::string data;
		data.append(tmp + rData->data_len, ntohs(rData->data_len));*/

		tmp += rData->data_len;
	}
	return true;
}


std::string YDNSResolver::readName(char* buff, int off, int& readLeng)
{
	std::string name;
	char* tmp = buff + off;
	bool isFrist = true;
	bool isJump = false;
	char strBuff[50] = { 0x00 };
	readLeng = 0;

	do {
		unsigned char num = *tmp++;
		if (num >= 0xc0)
		{
			int offset = num * 256 + (unsigned char)*(tmp) - 49152; //49152 = 11000000 00000000 ;)
			tmp = buff + offset;
			if (!isJump) readLeng += 2;
			isJump = true;
			continue;
		}

		if (num == '\0')
		{
			if (!isJump)  readLeng += 1;
			break;
		}
		if (isFrist)
		{
			isFrist = false;
		}
		else
		{
			name.append(".", 1);
		}
		memcpy(strBuff, tmp , num);
		strBuff[num] = '\0';
		name.append(strBuff, (size_t)num);
		tmp = tmp + num ;
		if (!isJump) readLeng += num + 1;
	} while (true);
	
	return name;
}

void YDNSResolver::changetoDnsNameFormat(char *dns, const char *host) {
	char* tmp = dns + strlen(host) + 1;
	*tmp-- = '\0';
	
	char num = 0;
	for (int i = strlen(host) - 1; i >= 0; --i)
	{
		if (*(host + i) == '.')
		{
			*tmp-- = num;
			num = 0;
		}else {
			*tmp-- = *(host + i);
			++num;
		}
	}

	*tmp = num;
}