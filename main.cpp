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
////소켓 정보 저장 구조체
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
////작업자 스레드 함수
//DWORD WINAPI WorkerThread(LPVOID arg);
//
////오류 출력함수
//void err_quit(TCHAR *msg);
//void err_display(char *msg);
//
////자신 IP확인
//void myIP();
//
//int main(int argc, char *argv[])
//{
//	myIP();
//	int retval;
//
//	//윈속 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }
//
//	//입출력 완료 포트 생성
//	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//	if(hcp == NULL) { return 1; }
//
//	//CPU 갯수 확인 (CPU 갯수에 비례하여 작업자 스레드 생성을 위한것이라고 한다.)
//	SYSTEM_INFO si;
//	GetSystemInfo(&si);
//
//	//(CPU * 2 ) 개의 작업자 스레드 생성
//	HANDLE hThread;
//	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
//		//스레드 함수 인자로 [입출력 완료 포트] 핸들 값 전달
//		//[입출력 완료 포트]에 접근하는 별도의 스레드->작업자 스레드(WorkerThread)
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
	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	SOCKET socket;


	std::vector < flatbuffers::Offset<testPlayer>>  _player;

	//def name
	char* name1 = new char[13]{ "JeongWonChae" };

	//flatbufferbuilder 생성
	flatbuffers::FlatBufferBuilder builder;
	
	//입출력 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) { return 1; }

	//thread 생성 갯수는 cpu * 2
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

	//Encode에 대해서 알아보기.
	//Packet

	system("pause");

	return 0;
}

DWORD WINAPIWorkerThread(LPVOID arg)
{

	return NULL;
}