#include "../TCPLib/TCPCommunication.h"
#include "../Common/Header.h"
#include "../ListLib/ListMethods.h"

int SendPacket(SOCKET socket, char* message, int messageSize)
{
	int sent = 0;
	int msgSize = messageSize;
	
	do
	{
		int iResult = send(socket, message + sent, messageSize - sent, 0);
		if (iResult == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		sent += iResult;
		msgSize -= iResult;
		if (msgSize < 0)
		{
			return -1;
		}
	} while (msgSize > 0);

	return 1;
}


int RecievePacket(SOCKET socket, char* recvBuffer, int length)
{
	int received = 0;
	int len = length;
	
	do
	{
		int iResult = recv(socket, recvBuffer + received, length - received, 0);

		if (iResult > 0)
		{
			received += iResult;
		}
		else if (iResult == 0)
		{
			// connection was closed gracefully
			printf("Connection with client closed.\n");
			closesocket(socket);
		}
		else
		{
			continue;
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			return WSAGetLastError();
		}
		len -= iResult;
		if (len < 0)
		{
			return -1;
		}
	} while (len > 0);

	return 1;
}
