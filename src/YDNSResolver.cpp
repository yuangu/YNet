#include "YDNSResolver.h"

#include "YUDPSocket.h"
#include "YAddress.h"

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

struct QUESTION {
	unsigned short qtype; //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	unsigned short qclass; //ͨ��Ϊ1��������Internet����
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
	ChangetoDnsNameFormat(qname, host.c_str());
	struct QUESTION *qinfo = (struct QUESTION *) buff + sizeof(struct DNS_HEADER) + host.length() + 2; //fill it

	qinfo->qtype = htons(1); //type of the query , A , MX , CNAME , NS etc  1A, 28AA
	qinfo->qclass = htons(1); //ͨ��Ϊ1��������Internet����

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