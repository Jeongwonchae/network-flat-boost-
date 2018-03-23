//#pragma commnet(lib, "ws2_32")
//#include<WinSock2.h>
//#include<cstdlib>
//#include<cstdio>
//#include <iostream>
//
//using namespace std;
//
//#define SERVERPORT 9000
//#define BUFSIZE 512
//
////���� ���� ���� ����ü
//struct SOCKETINFO
//{
//	OVERLAPPED overlapped;
//	SOCKET sock;
//	char vuf[BUFSIZE + 1];
//	int recvbytes;
//	int sendbytes;
//	WSABUF wsabuf;
//};
//
////�۾��� ������ �Լ�
//DWORD WINAPI WorkerThread(LPVOID arg);
//
////���� ����Լ�
//void err_quit(TCHAR *msg);
//void err_display(char *msg);
//
////�ڽ� IPȮ��
//void myIP();
//
//int main(int argc, char *argv[])
//{
//	myIP();
//	int retval;
//
//	//���� �ʱ�ȭ
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }
//
//	//����� �Ϸ� ��Ʈ ����
//	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//	if(hcp == NULL) { return 1; }
//
//	//CPU ���� Ȯ�� (CPU ������ ����Ͽ� �۾��� ������ ������ ���Ѱ��̶�� �Ѵ�.)
//	SYSTEM_INFO si;
//	GetSystemInfo(&si);
//
//	//(CPU * 2 ) ���� �۾��� ������ ����
//	HANDLE hThread;
//	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
//		//������ �Լ� ���ڷ� [����� �Ϸ� ��Ʈ] �ڵ� �� ����
//		//[����� �Ϸ� ��Ʈ]�� �����ϴ� ������ ������->�۾��� ������(WorkerThread)
//		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
//		if (hThread == NULL) { return 1; }
//		CloseHandle(hThread);
//	}
//
//	//SOCKET ()
//	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//	//if (listen_sock == INVALID_SOCKET) { err_quit(L"socket()"); }
//	return 0;
//}

//------------------------------------------------------------------------------------------
#pragma comment(lib, "ws2_32.lib")
#include <boost/thread.hpp>
#include <WinSock2.h>
#include <Windows.h>
#include "flatbuffers\flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/minireflect.h"
#include "flatbuffers/registry.h"
#include "../src/main/cpp/testProtocol_generated.h"

#include <iostream>

using namespace std;

DWORD WINAPIWorkerThread(LPVOID arg);

int main()
{
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	SOCKET socket;


	std::vector < flatbuffers::Offset<testPlayer>>  _player;

	//def name
	char* name1 = new char[13]{ "JeongWonChae" };

	//flatbufferbuilder ����
	flatbuffers::FlatBufferBuilder builder;
	
	//����� ��Ʈ ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) { return 1; }

	//thread ���� ������ cpu * 2
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	boost::thread* th = new boost::thread[(int)si.dwNumberOfProcessors];
	cout << (int)si.dwNumberOfProcessors <<  endl;
	for (int i = 0; i  < (int)si.dwNumberOfProcessors; i++)
	{
		th[i] = boost::thread(boost::bind(WINAPIWorkerThread, hcp));
		//if (th[i] == nullptr) { return 1; }
		cout << "thread number : [ " << i << " ] " << endl;
	}

	auto p1 = CreatetestPlayer(
		builder,
		100,
		20,
		builder.CreateSharedString(name1));

	_player.push_back(p1);

	char* name2 = new char[13]{ "JeongEunChae" };

	auto p2 = CreatetestPlayer(
		builder,
		110,
		0,
		builder.CreateSharedString(name2));

	_player.push_back(p2);

	auto players = builder.CreateVector(_player);

	builder.Finish(players);

	//serialize?
	uint8_t* buf = builder.GetBufferPointer();
	uint32_t size = builder.GetSize();

	//send(socket,)

	//Encode�� ���ؼ� �˾ƺ���.
	//Packet

	system("pause");

	return 0;
}

DWORD WINAPIWorkerThread(LPVOID arg)
{

	return NULL;
}