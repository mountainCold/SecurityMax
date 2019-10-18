#pragma once
#include <cstdint>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
extern HWND hDlgB;
enum DataPackType {
	selectType = 1, //查询
	insertType = 2,//增加

};
#pragma pack(push,1) //一字节对齐
struct DataPack
{
	DataPackType type;
	uint32_t	 size;
};
struct DataPackResult
{
	uint32_t type;		//回复类型
	int status; //状态码
	uint32_t size;	 //大小
	char data[1]; //只给一个字节进行越界操作
};
#pragma pack(pop) //恢复默认对齐方式
class Client {
	SOCKET m_hSocket;	
public:

	/*初始化套接字环境
	*/
	static void initSocket();
	/*创建连接服务端
	*/
	Client(const char* serverIp, short port);
	/*发送消息
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