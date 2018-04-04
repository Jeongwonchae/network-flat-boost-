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
#pragma warning(disable:4996)
#include <boost/thread.hpp>
#include <WinSock2.h>
#include <Windows.h>
#include "flatbuffers\flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/minireflect.h"
#include "flatbuffers/registry.h"
#include "../src/main/cpp/testProtocol_generated.h"
#include <flatbuffers\util.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

const int SERVERPORT = 9000;
const int BUFSIZE = 512;

DWORD WINAPIWorkerThread(LPVOID arg);

void errLog(char* msg);

//소켓정보 저장을 위한 구조체<이게 flatbuffer가 해주는 일 아닌가?
struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

void myIP();

int main()
{
	myIP();

	int retval;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	//SOCKET socket;

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
	cout << "CPU Count : " << (int)si.dwNumberOfProcessors <<  endl;
	for (int i = 0; i  < (int)si.dwNumberOfProcessors; i++)
	{
		th[i] = boost::thread(boost::bind(WINAPIWorkerThread, hcp));
		//if (th[i] == nullptr) { return 1; }

		//cout << "thread number : [ " << i << " ] " << endl;
	}

	//Listen Socket()
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	{
		char errormsg[] = "socket()";
		if (listen_socket == INVALID_SOCKET) { errLog(errormsg); }
	}

	//bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = ::bind(listen_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	{
		char errormsg[] = "bind()";
		if (retval == SOCKET_ERROR) { errLog(errormsg); }
	}

	//listen()
	retval = listen(listen_socket, SOMAXCONN);
	{
		char errormsg[] = "listen()";
		if (retval == SOCKET_ERROR) { errLog(errormsg); }
	}

	//데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_socket, (SOCKADDR*)&clientaddr, &addrlen);
		{
			char errormsg[] = "accept()";
			if (client_sock == INVALID_SOCKET) { errLog(errormsg); break; }
		}

		//소켓과 입출력 완료 포트 연결
		//accept()함수가 리턴한 소켓(client_sock)을 [입출력 오나료 포트]와 연결
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		//소켓 정보 구조체 할당
		SOCKETINFO *ptr = new SOCKETINFO;
		if (ptr == NULL) { break; }

		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		//비동기 입출력 시작 (WSARecv 호출)
		flags = 0;
		retval = WSARecv(client_sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) {
			{
				char errormsg[] = "WSARecv()";
				if (WSAGetLastError() != ERROR_IO_PENDING) { errLog(errormsg); }
				continue;
			}
		}
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
	uint64_t size = builder.GetSize();

	//send(socket,)

	//Encode에 대해서 알아보기.
	//Packet

	system("pause");

	return 0;
}

DWORD WINAPIWorkerThread(LPVOID arg)
{
	int retval;

	HANDLE hcp = (HANDLE)arg;

	while (1)
	{
		//비동기 입출력 완료 기다리기 (GetQueuedCompletionSatus 함수 호출)
		DWORD cbTransferred;
		SOCKET client_sock;
		SOCKETINFO *ptr;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, &client_sock, (LPOVERLAPPED*)&ptr, INFINITE);
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (retval == 0 || cbTransferred == 0)
		{
			if (retval == 0)
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &temp1, FALSE, &temp2);
				{
					char errormsg[] = "WSAGetOverleappedRTesult()";
					errLog(errormsg);
				}
			}
			closesocket(ptr->sock);
			cout << "[TCP 서버]클라이언트 종료 : IP 주소 = " << inet_ntoa(clientaddr.sin_addr) << ", 포트 번호 = " <<
				ntohs(clientaddr.sin_port) << endl;

			delete ptr;
			continue;
		}

		//데이터 전송량 갱신
		//소켓 정보 구조체를 참조하면, '받은 데이터' 인지 '보낸 데이터' 인지 확인가능
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred; // 198행
			ptr->sendbytes = 0;

			//받은 데이터 출력
			ptr->buf[ptr->recvbytes] = '\0';

			cout << "[IP = " << inet_ntoa(clientaddr.sin_addr) << " Port = " << ntohs(clientaddr.sin_port) << " ] " << ptr->buf;
		}
		else
		{
			ptr->sendbytes += cbTransferred; //206행
		}

		//보낸 데이터가 받은 데이터보다 적으면, 아직 보내지 못한 데이터를 마저 보냄

		if (ptr->recvbytes > ptr->sendbytes) {
			//데이터 보내기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			DWORD sendbytes;
			//WSASend() 함수는 비동기적으로 동작하기 때문에, 실제 보낸 데이터 수는 다음 루프 때 확인이 가능하다고 한다. 206행
			retval = WSASend(ptr->sock, &ptr->wsabuf, 1, &sendbytes, 0, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					{
						char errormsg[] = "WSASend()";
						errLog(errormsg);
					}
				}
				continue;
			}
			else
			{
				ptr->recvbytes = 0;
				//데이터 받기
				//소켓 정보 중 받은 데이터 수를 초기화 한 후
				ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
				ptr->wsabuf.buf = ptr->buf;
				ptr->wsabuf.len = BUFSIZE;

				DWORD recvbytes;
				DWORD flags = 0;
				//도착한 데이터를 읽음
				//WSARecv()함수도 비동기 이므로, 다음 루프 시 확인 가능 -> 198행 코드
				retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
				if (retval == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING)
					{
						{
							char errormsg[] = "WSARecv()";
							errLog(errormsg);
						}
					}
					continue;
				}
			}
		}


	}
	puts("Inner Thread");
	return NULL;
}

void myIP()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];

	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int nError = gethostname(hostname, sizeof(hostname));

	if (nError == 0) {
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
	}

	WSACleanup();
	cout << "This Computer's IP address : " << ipaddr << endl;
}

void errLog(char* msg)
{

}