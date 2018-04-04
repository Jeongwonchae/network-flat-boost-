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

//�������� ������ ���� ����ü<�̰� flatbuffer�� ���ִ� �� �ƴѰ�?
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

	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	//SOCKET socket;

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

	//������ ��ſ� ����� ����
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

		//���ϰ� ����� �Ϸ� ��Ʈ ����
		//accept()�Լ��� ������ ����(client_sock)�� [����� ������ ��Ʈ]�� ����
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		//���� ���� ����ü �Ҵ�
		SOCKETINFO *ptr = new SOCKETINFO;
		if (ptr == NULL) { break; }

		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		//�񵿱� ����� ���� (WSARecv ȣ��)
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

	//Encode�� ���ؼ� �˾ƺ���.
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
		//�񵿱� ����� �Ϸ� ��ٸ��� (GetQueuedCompletionSatus �Լ� ȣ��)
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
			cout << "[TCP ����]Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientaddr.sin_addr) << ", ��Ʈ ��ȣ = " <<
				ntohs(clientaddr.sin_port) << endl;

			delete ptr;
			continue;
		}

		//������ ���۷� ����
		//���� ���� ����ü�� �����ϸ�, '���� ������' ���� '���� ������' ���� Ȯ�ΰ���
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred; // 198��
			ptr->sendbytes = 0;

			//���� ������ ���
			ptr->buf[ptr->recvbytes] = '\0';

			cout << "[IP = " << inet_ntoa(clientaddr.sin_addr) << " Port = " << ntohs(clientaddr.sin_port) << " ] " << ptr->buf;
		}
		else
		{
			ptr->sendbytes += cbTransferred; //206��
		}

		//���� �����Ͱ� ���� �����ͺ��� ������, ���� ������ ���� �����͸� ���� ����

		if (ptr->recvbytes > ptr->sendbytes) {
			//������ ������
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			DWORD sendbytes;
			//WSASend() �Լ��� �񵿱������� �����ϱ� ������, ���� ���� ������ ���� ���� ���� �� Ȯ���� �����ϴٰ� �Ѵ�. 206��
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
				//������ �ޱ�
				//���� ���� �� ���� ������ ���� �ʱ�ȭ �� ��
				ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
				ptr->wsabuf.buf = ptr->buf;
				ptr->wsabuf.len = BUFSIZE;

				DWORD recvbytes;
				DWORD flags = 0;
				//������ �����͸� ����
				//WSARecv()�Լ��� �񵿱� �̹Ƿ�, ���� ���� �� Ȯ�� ���� -> 198�� �ڵ�
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