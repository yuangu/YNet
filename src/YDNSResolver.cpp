#include "YDNSResolver.h"

#include "YUDPSocket.h"
#include "YAddress.h"

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

struct QUESTION {
	unsigned short qtype; //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	unsigned short qclass; //通常为1，表明是Internet数据
};

#pragma pack(push, 1)
struct R_DATA {
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD {
	unsigned char* name;
	struct R_DATA *resource;
	unsigned char* rdata;
};

bool YDNSResolver::lookupByName(std::string& host)
{	
	size_t size = sizeof(DNS_HEADER) + sizeof(QUESTION) + host.length() + 2;
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
	ChangetoDnsNameFormat(qname, host.c_str());
	struct QUESTION *qinfo = (struct QUESTION *) buff + sizeof(struct DNS_HEADER) + host.length() + 2; //fill it

	qinfo->qtype = htons(1); //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	qinfo->qclass = htons(1); //通常为1，表明是Internet数据

	YUDPSocket client;
	YAddress address(std::string("8.8.8.8"), 53);
	client.sendTo(&address, buff, size);

	char _buff[1024] = {0x00};
	client.receiveFrom(&address, _buff, 1024);

	DNS_HEADER* header = (DNS_HEADER*)_buff;
	header->id;
	header->qr;

	char* tmp = _buff + size;
	bool isFrist = true;
	while (*(tmp++) != '\0')
	{
		isFrist = false;
	}
	if (isFrist)
	{
		tmp++;
	}

	QUESTION *q = (QUESTION *)tmp;
	tmp += sizeof(QUESTION);

	int *num = (int *)tmp;
	int ttl = htonl(*num);
	tmp += 4;

	unsigned short* num2 = (unsigned short*)tmp;
	unsigned short dataLen = htons(*num2);
	tmp += 2;
	std::string d = std::string(tmp, dataLen);

	return true;
}


void YDNSResolver::ChangetoDnsNameFormat(char *dns, const char *host) {
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