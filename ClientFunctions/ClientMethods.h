#pragma once
//#pragma warning(disable:4996) 
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

bool TryRegistration(SOCKET connectSocket, char* messageToSend);

bool SendRegistration(char* pStr, SOCKET connectSocket, char* messageToSend);

void ChooseTypeOfMessageToSend(SOCKET connectSocket);

void ChooseTypeOfMessageToSendForPlayer(SOCKET connectSocket);

void SendIntervalToServer(SOCKET connectSocket);

void ReceiveMessageThroughServer(char* serialized);

