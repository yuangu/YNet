#include "YDNSResolver.h"
#include "YAddress.h"
#include "YUDPSocket.h"
#include "YBinaryStream.h"

#define DNS_SERVER_PORT 53

//https://jocent.me/2017/06/18/dns-protocol-principle.html#_label1_0
struct DNS_HEADER {
	unsigned short id; // �Ự������ư���

	unsigned char rd : 1; // ��ʾ�����ݹ�
	unsigned char tc : 1; // ��ʾ�ɽضϵ�
	unsigned char aa : 1; // ��ʾ��Ȩ�ش�
	unsigned char opcode : 4; // 0��ʾ��׼��ѯ��1��ʾ�����ѯ��2��ʾ������״̬����
	unsigned char qr : 1; // ��ѯ/��Ӧ��־��0Ϊ��ѯ��1Ϊ��Ӧ

	unsigned char rcode : 4; // ��ʾ�����룬0��ʾû�в��3��ʾ���ֲ��2��ʾ����������Server Failure��
	unsigned char cd : 1; // checking disabled
	unsigned char ad : 1; // authenticated data
	unsigned char z : 1; // its z! reserved
	unsigned char ra : 1; // ��ʾ���õݹ�

	unsigned short q_count; // ��ʾ��ѯ��������ڵ�����
	unsigned short ans_count; //Answers��ʾ�ش����������
	unsigned short auth_count; // ��ʾ��Ȩ���������
	unsigned short add_count; // ��ʾ�������������
};

#pragma pack(push, 1)
struct QUESTION {
	unsigned short qtype; //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	unsigned short qclass; //ͨ��Ϊ1��������Internet����
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

	dns->id = (unsigned short)100;  //�Ựid
	dns->qr = 0; //0��ʾ���� 1������Ӧ
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
	qinfo->qclass = htons(1); //ͨ��Ϊ1��������Internet����
	
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
	
	//��������
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