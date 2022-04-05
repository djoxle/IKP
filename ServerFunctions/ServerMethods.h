#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "../ListLib/ListMethods.h"
#include "../Common/Header.h"
#include "../TCPLib/TCPCommunication.h"

void ConfirmRegistration(char* serialized, NODE* client, NODE* head);

void ForwardMessageThroughServer(char* serialized, NODE* head, NODE* client);

void ForwardGameoverMessageThroughServer(char* serialized, NODE* head);

void RequestToBePlayer(char* serialized, NODE* head, NODE* client);

void AcceptInterval(char* serialized, NODE* head, NODE* client);

