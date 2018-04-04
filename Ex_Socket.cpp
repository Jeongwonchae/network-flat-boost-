#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>


DWORD WINAPI SendThread( LPVOID lpData );

char ID[10];

int main()
{
	// ���� ���̺귯�� �ʱ�ȭ
	WSADATA wsaData;
	int token = WSAStartup( WINSOCK_VERSION, &wsaData );
	char PORT[5], IP[15];

	// ���� ����
	SOCKET socket_client = socket( AF_INET, SOCK_STREAM, 0 );

	printf("����� IP �ּ� : ");
	scanf("%s", &IP);

	printf("����� ��Ʈ ��ȣ : ");
	scanf("%s", &PORT);

	// ���� �ּ� ����
	SOCKADDR_IN servAddr={0};
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(IP);
	servAddr.sin_port=htons( atoi(PORT) ); // ��Ʈ

	// ���� ����
	if( connect( socket_client, (struct sockaddr *) &servAddr, sizeof(servAddr) ) == SOCKET_ERROR )
	{
		closesocket( socket_client );
		printf("������ ���� �Ҽ� �����ϴ�.");
		return SOCKET_ERROR;
	}

	CreateThread( NULL, 0, SendThread, (LPVOID)socket_client, 0, NULL );
	// ���� ���
	DWORD dwTmp;
	char text[1024] = "������ ���ӵǾ����ϴ�.\n";
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, strlen(text) , &dwTmp, NULL);

	// ���� & ���� ��Ǫ
	while( 1 )
	{
		char buffer[1024]={0};
		int len = recv( socket_client, buffer, sizeof(buffer), 0 );
		if ( len <= 0 )
		{
			printf("���� ����.");
			break;
		}

		// �޽��� ���
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer) , &dwTmp, NULL);
	}
	//���� ����
	closesocket( socket_client );
	return 0;
}


DWORD WINAPI SendThread( LPVOID lpData )
{
	SOCKET socket_client = (SOCKET)lpData;
	while( 1 )
	{
		printf("������ ���� : ");
		DWORD dwTmp;
		char text[1024]={0};
		ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, 1024 , &dwTmp, NULL);
		send( socket_client, text, strlen(text), 0 );
	}
	return 0;
}
