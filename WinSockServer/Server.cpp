#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../ListLib/ListMethods.h"
#include "../TCPLib/TCPCommunication.h"
#include "../Common/Header.h"
#include "../ServerFunctions/ServerMethods.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016" 

bool InitializeWindowsSockets();

int  main(void) 
{
    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;
    // Socket used for communication with client
    SOCKET acceptedSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // Buffer used for storing incoming data
    char recvbuf[DEFAULT_BUFLEN];

	NODE* head;
	InitList(&head);
    
    if(InitializeWindowsSockets() == false)
    {
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
    }
    
    // Prepare address information structures
    addrinfo *resultingAddress = NULL;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
    if ( iResult != 0 )
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address famly
                          SOCK_STREAM,  // stream socket
                          IPPROTO_TCP); // TCP

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address 
    // to socket
    iResult = bind( listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

	printf("Server initialized, waiting for clients.\n");

	unsigned long mode = 1; //non-blocking mode
	iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	fd_set readfds;
	FD_ZERO(&readfds);

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

    do
    {
		FD_SET(listenSocket, &readfds);
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
			if (FD_ISSET(listenSocket, &readfds))
			{
				SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);

				Client c = MakeNewClient("naziv klijenta", acceptedSocket);

				Push(&head, c);

				if (acceptedSocket == INVALID_SOCKET)
				{
					printf("accept failed with error: %d\n", WSAGetLastError());
					closesocket(listenSocket);
					WSACleanup();
					return 1;
				}

				mode = 1; //non-blocking mode
				iResult = ioctlsocket(acceptedSocket, FIONBIO, &mode);
				if (iResult != NO_ERROR)
					printf("ioctlsocket failed with error: %ld\n", iResult);

			}
		}

		fd_set readfds2;
		FD_ZERO(&readfds2);

		timeval timeVal2;
		timeVal2.tv_sec = 1;
		timeVal2.tv_usec = 0;

		NODE* pom1 = head;
		while (pom1 != NULL) //prolazim kroz listu dok ne dodjem do kraja
		{
			FD_SET(pom1->client_data.acceptedSocket, &readfds2);
			pom1 = pom1->next;
		}

		result = select(0, &readfds2, NULL, NULL, &timeVal2);

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
			NODE* pom2 = head;
			while (pom2 != NULL && result > 0) //prolazim kroz listu dok ne dodjem do kraja
			{
				if (FD_ISSET(pom2->client_data.acceptedSocket, &readfds2))
				{
					result--;
					int len;

					iResult = RecievePacket(pom2->client_data.acceptedSocket, (char*)(&len), 4);
					if (iResult == -1)
					{
						printf("Greska prilikom prijema duzine poruke.");
						return 1;
					}

					char* serialized = (char*)malloc(len);

					iResult = RecievePacket(pom2->client_data.acceptedSocket, serialized, len);
					if (iResult == -1)
					{
						printf("Greska prilikom prijema poruke");
					}

					int tipPoruke;

					char* yval = serialized;

					tipPoruke = *((int*)yval);

					switch (tipPoruke) 
					{
						case 0:
						{
							ConfirmRegistration(serialized, pom2, head);

							break;
						}
						case 1:
						{
							ForwardMessageThroughServer(serialized, head, pom2);

							break;
						}
							
						case 2:
						{
							RequestToBePlayer(serialized, head, pom2);

							break;
						}
						case 3:
						{
							AcceptInterval(serialized, head, pom2);

							break;
						}
						case 5:
						{
							ForwardGameoverMessageThroughServer(serialized, head);

							break;
						}
							
						default: break;
					}

					free(serialized);
					serialized = NULL;
					
				}

				pom2 = pom2->next;
			}
		}

		FD_CLR(listenSocket, &readfds);
		NODE* pom3 = head;
		while (pom3 != NULL)
		{
			FD_CLR(pom3->client_data.acceptedSocket, &readfds2);
			pom3 = pom3->next;
		}

    } while (1);

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
