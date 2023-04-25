#pragma warning(disable : 4996)
#pragma warning(disable : 6385)

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define PORT_1_SPACE "   "
#define PORT_2_SPACE "              "
#define PORT_3_SPACE "                         "
#define PORT_4_SPACE "                                    "

#define PORT_1_BORDER "                                    |"
#define PORT_2_BORDER "                         |"
#define PORT_3_BORDER "              |"
#define PORT_4_BORDER "   |"

#define PORT_1 10001
#define PORT_2 10002
#define PORT_3 10003
#define PORT_4 10004

#define SLOT_INTERVAL 100
#define END_CHAR 255

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
	((byte) & 0x80 ? '1' : '0'), \
	((byte) & 0x40 ? '1' : '0'), \
	((byte) & 0x20 ? '1' : '0'), \
	((byte) & 0x10 ? '1' : '0'), \
	((byte) & 0x8 ? '1' : '0'), \
	((byte) & 0x4 ? '1' : '0'), \
	((byte) & 0x2 ? '1' : '0'), \
	((byte) & 0x1 ? '1' : '0')

int main(int argc, char** argv)
{
	system("cls");

	HWND hConsole = GetForegroundWindow();
	SetWindowText(hConsole, "Sender");

	if (argc != 3)
	{
		printf("[-] Wrong format\n");
		printf("[*] Correct format: {EXE} -t \"{TEXT}\"\n");
		printf("[*] Correct format: {EXE} -f \"{TEXT FILE PATH}\"\n");
		return -1;
	}

	if (!*argv[2])
	{
		printf("[-] Data cannot not be empty\n");
		return -1;
	}

	char end[2] = { END_CHAR, 0 };
	unsigned int msgSize = 2;
	char* msg;

	if (!strcmp(argv[1], "-t"))
	{
		msgSize += sizeof(char) * strlen(argv[2]);
		msg = (char*)malloc(msgSize);
		strcpy_s(msg, msgSize, argv[2]);
	}
	else if (!strcmp(argv[1], "-f"))
	{
		HANDLE hFile = CreateFile(
			argv[2],
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		msgSize += (unsigned int)GetFileSize(hFile, NULL);
		msg = (char*)malloc(msgSize);

		LPDWORD bytesRead = NULL;
		if (!ReadFile(hFile, msg, msgSize, bytesRead, NULL))
		{
			printf("[-] Failed to open file\n");
			return -1;
		}

		msg[msgSize - 2] = '\0';
	}
	else
	{
		printf("[-] Wrong format\n");
		printf("[*] Correct format: {EXE} -t \"{TEXT}\"\n");
		printf("[*] Correct format: {EXE} -f \"{TEXT FILE PATH}\"\n");
		return -1;
	}

	strcat(msg, end);

	int currentPort = 0;
	int i = 0;

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("[-] WSAStartup failed\n");
		return -1;
	}

	SOCKET sock;
	struct sockaddr_in server;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("[-] Socket creation failed\n");
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	char* portSpace;
	char* border;

	printf("_________P1_________P2_________P3_________P4______\n");
	while (*msg)
	{
		switch (currentPort)
		{
		case 0:
			portSpace = PORT_1_SPACE;
			border = PORT_1_BORDER;
			server.sin_port = htons(PORT_1);
			break;
		case 1:
			portSpace = PORT_2_SPACE;
			border = PORT_2_BORDER;
			server.sin_port = htons(PORT_2);
			break;
		case 2:
			portSpace = PORT_3_SPACE;
			border = PORT_3_BORDER;
			server.sin_port = htons(PORT_3);
			break;
		case 3:
			portSpace = PORT_4_SPACE;
			border = PORT_4_BORDER;
			server.sin_port = htons(PORT_4);
			break;
		default:
			portSpace = NULL;
			border = NULL;
			break;
		}

		printf("%u |%s"BYTE_TO_BINARY_PATTERN"%s\n", i % 10, portSpace, BYTE_TO_BINARY(*msg), border);

		sendto(sock, msg, 1, 0, (struct sockaddr*)&server, sizeof(server));

		currentPort = (short)*(msg++) % 4;
		i++;

		Sleep(SLOT_INTERVAL);
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}