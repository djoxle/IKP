#include "../ListLib//ListMethods.h"


void InitList(NODE** head)
{
	*head = NULL;
}


void Push(NODE** head, Client c)
{
	NODE* temp = *head;

	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->next = NULL;
	newNode->client_data = c;

	if (*head == NULL)
	{
		*head = newNode;
		return;
	}
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newNode;
}

void ClearList(NODE** head)
{
	NODE* temp = *head;
	while (temp != NULL)
	{
		*head = temp->next;

		free(temp);

		temp = *head;
	}
}

Client MakeNewClient(char* name, SOCKET acceptedSocket)
{
	Client client;

	strcpy_s(client.name, name);
	client.acceptedSocket = acceptedSocket;
	client.isPlayer = false;

	return client;
}

