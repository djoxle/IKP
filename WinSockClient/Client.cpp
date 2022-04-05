#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "../TCPLib/TCPCommunication.h"
#include "../Common/Header.h"
#include "../ClientFunctions/ClientMethods.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

int __cdecl main() 
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    char messageToSend[50];

    if(InitializeWindowsSockets() == false)
    {
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
                           SOCK_STREAM,
                           IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }
 
	unsigned long mode = 1; //non-blocking mode
	iResult = ioctlsocket(connectSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	fd_set readfds;
	FD_ZERO(&readfds);

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	CLIENT_SESSION_DATA session;
	session.isRegistered = false;
	session.isPlayer = false;
	session.isGameOver = false;

	while (true)
	{
		if (!session.isRegistered) 
		{
			session.isRegistered = TryRegistration(connectSocket, messageToSend);
		}
		// U ovaj else se ulazi ako je klijent registrovan
		else 
		{
			printf("\nPritisnite taster 'n' za novu poruku!");
			while (!kbhit())
			{	
				FD_SET(connectSocket, &readfds);
				int result = select(0, &readfds, NULL, NULL, &timeVal);

				if (result == 0)
				{
					// vreme za cekanje je isteklo
				}
				else if (result == SOCKET_ERROR)
				{
					//desila se greska prilikom poziva funkcije
				}
				else
				{
					if (FD_ISSET(connectSocket, &readfds))
					{
						int len;

						iResult = RecievePacket(connectSocket, (char*)(&len), 4);
						if (iResult == -1)
						{
							printf("Greska prilikom prijema duzine poruke.");
							return 1;
						}

						char* serialized = (char*)malloc(len);

						iResult = RecievePacket(connectSocket, serialized, len);
						if (iResult == -1)
						{
							printf("Greska prilikom prijema poruke");
						}

						int tipPoruke;

						char* yval = serialized;

						tipPoruke = *((int*)yval);

						switch (tipPoruke)
						{
							case 1:
							{
								ReceiveMessageThroughServer(serialized);

								printf("\nPritisnite taster 'n' za novu poruku!");

								break;
							}
							case 2:
							{
								session.isPlayer = true;
								printf("\nPostali ste igrac!");

								SendIntervalToServer(connectSocket);
								break;
							}
							case 3: {
								printf("\nVas interval je primljen!");
								break;
							}
							case 4:
							{
								printf("\nIgrac je vec izabran!");
								break;
							}
							case 5: 
							{
								printf("\nBroj je pogodjen, igra je zavrsena");
								session.isGameOver = true;
								break;
							}
							default: break;
						}

						free(serialized);
						serialized = NULL;

					}
				}
				FD_CLR(connectSocket, &readfds);
			}

			if (session.isGameOver)
				break;
			
			if (getch() != 'n')
				continue;

			if (session.isPlayer)
				ChooseTypeOfMessageToSendForPlayer(connectSocket);
			else
				ChooseTypeOfMessageToSend(connectSocket);

		}

		FD_CLR(connectSocket, &readfds);

		if (session.isGameOver)
			break;
	}

    // cleanup
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
	return true;
}
