#pragma once
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Client_st
{
	char name[512];
	SOCKET acceptedSocket;
	bool isPlayer;
}Client;

typedef struct Node_st
{
	Client client_data;
	struct Node_st* next;
}NODE;


void InitList(NODE** head);

void Push(NODE** head, Client c);

void ClearList(NODE** head);

Client MakeNewClient(char* name, SOCKET acceptedSocket);
