#include "pch.h"
#include "Client.h"
#include <Ws2tcpip.h>
#include <regex>
#include <string.h>
#include <atlstr.h>
HWND hDlgB = NULL;
void Client::initSocket()
{
	//WSADATA wd = { 0 };
	//WSAStartup(MAKEWORD(2, 2), &wd);
}

Client::Client(const char* serverIp, short port)
{
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wd);
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.S_un.S_addr = inet_addr(serverIp);
	inet_pton(AF_INET, serverIp, &addr.sin_addr);
	int size =connect(m_hSocket, (sockaddr*)& addr, sizeof(addr));
	if (size == SOCKET_ERROR)
	{
		MessageBox(0, L"���ӷ�����ʧ��", 0, 0);
	}
	
}

void Client::send(DataPackType type, const char* data, int size)
{
	if (size ==-1)
	{
		size = strlen(data);
	}
	DataPack dp = { type,size };		
	::send(m_hSocket, (char*)& dp, sizeof(dp), 0);
	::send(m_hSocket, data, size, 0);
}

DataPackResult* Client::recv()
{
	DataPackResult head = { 0 };
	//û��data����
	if (::recv(m_hSocket, (char*)& head, sizeof(head) - 1, 0) != sizeof(head) - 1) {
		return NULL;
	}
	DataPackResult* pBuff = (DataPackResult*)malloc(sizeof(DataPackResult) + head.size);
	memset(pBuff, 0, head.size + sizeof(DataPackResult));
	memcpy(pBuff, &head, sizeof(head));
	if (::recv(m_hSocket, pBuff->data, pBuff->size, 0) != pBuff->size) {
		free(pBuff);
		return NULL;
	}
	return (DataPackResult*)pBuff;
}

void Client::freeResult(DataPackResult* p)
{
	free(p);
}
void Select(Client* client)
{
	CStringA buffer;
	buffer.Format("%s", "��������");
	client->send(selectType,
		buffer,
		buffer.GetLength());
}
void Insert(Client* client,
	const char* name,
	const char* MD5)
{
	CStringA buffer;
	buffer.Format("%s\x11%s", name, MD5);
	client->send(insertType,
		buffer,
		buffer.GetLength());
}
DWORD recvProc1(LPVOID arg)
{
	Client* pClient = (Client*)arg;
	DataPackResult* pResult = NULL;
	pResult = pClient->recv();
	if (pResult->status > 0) {
		char buffer[200] = { 0 };
		sprintf_s(buffer, 200, "[%d] �Ĵ���ʧ��:%s\n", pResult->type, pResult->data);
		MessageBoxA(0, buffer, 0, 0);
	}
	switch (pResult->type)
	{
	case selectType:
	{
		CStringA  buffer;
		buffer.Format("%d\\%s\\", selectType, pResult->data);
		SendToWindow(hDlgB, buffer);
	}
	break;
	case insertType:
	{
		MessageBox(hDlgB, L"��ӳɹ�", L"�����ʾ", 0);
		break;
	}
	default:
		break;
	}
	return 0;
}
void SendToWindow(HWND hFind, const char* data, int status)
{
	//����copydata��Ϣ
	COPYDATASTRUCT stcData = {};
	stcData.dwData = 0x100;						//�ĸ��ֽ�����ֵ
	stcData.lpData = (LPVOID)data;		//������
	stcData.cbData = strlen(data);						//��������С
	//2. ����WM_COPYDATA����
	SendMessage(hFind, WM_COPYDATA, NULL, (LPARAM)& stcData);
}
Client client("127.0.0.1", 10086);