#pragma warning(disable : 4996)
#pragma warning(disable : 6001)

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define PORT_1 10001
#define PORT_2 10002
#define PORT_3 10003
#define PORT_4 10004
#define BUFFER_SIZE 2
#define END_CHAR (char)255

int main()
{
	system("cls");

	HWND hConsole = GetForegroundWindow();
	SetWindowText(hConsole, "Receiver");

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("[-] WSAStartup failed\n");
		return -1;
	}

	SOCKET sock;
	int slen, recv_len, currentPort = 0;
	char buff[BUFFER_SIZE];
	struct sockaddr_in server, client;

	slen = sizeof(client);
	memset(&server, 0, sizeof(server));
	
	while (TRUE)
	{
		if (*buff == END_CHAR)
		{
			printf("\n");
			currentPort = 0;
		}

		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			printf("[-] Socket creation failed\n");
			return -1;
		}

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;

		switch (currentPort)
		{
		case 0:
			server.sin_port = htons(PORT_1);
			break;
		case 1:
			server.sin_port = htons(PORT_2);
			break;
		case 2:
			server.sin_port = htons(PORT_3);
			break;
		case 3:
			server.sin_port = htons(PORT_4);
			break;
		}

		if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
		{
			printf("[-] Bind failed\n");
			return -1;
		}

		if ((recv_len = recvfrom(sock, buff, BUFFER_SIZE, 0, (struct sockaddr*)&client, &slen)) < 0)
		{
			printf("[-] Failed to receive\n");
			return -1;
		}
		buff[BUFFER_SIZE - 1] = 0;

		printf("%s", buff);

		currentPort = (short)*buff % 4;
		closesocket(sock);
	}

	WSACleanup();

	return 0;
}