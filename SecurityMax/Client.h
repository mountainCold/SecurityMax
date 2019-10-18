#pragma once
#include <cstdint>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
extern HWND hDlgB;
enum DataPackType {
	selectType = 1, //��ѯ
	insertType = 2,//����

};
#pragma pack(push,1) //һ�ֽڶ���
struct DataPack
{
	DataPackType type;
	uint32_t	 size;
};
struct DataPackResult
{
	uint32_t type;		//�ظ�����
	int status; //״̬��
	uint32_t size;	 //��С
	char data[1]; //ֻ��һ���ֽڽ���Խ�����
};
#pragma pack(pop) //�ָ�Ĭ�϶��뷽ʽ
class Client {
	SOCKET m_hSocket;	
public:

	/*��ʼ���׽��ֻ���
	*/
	static void initSocket();
	/*�������ӷ����
	*/
	Client(const char* serverIp, short port);
	/*������Ϣ
	*/
	void send(DataPackType type, const char* data, int size = -1);
	DataPackResult* recv();
	void freeResult(DataPackResult* p);
};
void Select(Client* client);
void Insert(Client* client, const char* name, const char* MD5);
DWORD recvProc1(LPVOID arg);
void SendToWindow(HWND hFind, const char* data, int status = 0);
extern Client client;