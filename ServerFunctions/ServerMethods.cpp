#include "../ServerFunctions/ServerMethods.h"

void ConfirmRegistration(char* serialized, NODE* client, NODE* head)
{
	char* yval = serialized;
	char* metadata = yval + sizeof(int);
	char* ystr = metadata + sizeof(int);
	REGISTRATION regDes;
	int sLen = *((int*)metadata);
	regDes.id = *((int*)yval);
	regDes.name = (char*)malloc((sLen + 1) * sizeof(char));
	strncpy_s(regDes.name, sLen + 1, ystr, sLen);
	regDes.name[sLen] = '\0';

	bool clientExist = false;

	// proveri da li vec postoji klijenta sa tim nazivom
	NODE* pom = head;
	while (pom != NULL) //prolazim kroz listu dok ne dodjem do kraja
	{
		if (!strcmp(pom->client_data.name, regDes.name))
		{
			clientExist = true;
			break;
		}
		pom = pom->next;
	}

	if (clientExist)
	{
		char potvrda[22] = "Korisnik vec postoji!";


		// ZAKOMENTARISATI AKO PRAVI PROBLEM
		free(regDes.name);
		regDes.name = NULL;

		int iResult = SendPacket(client->client_data.acceptedSocket, potvrda, strlen(potvrda));
		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja podataka.\n");
		}
	}
	else
	{
		char potvrda[22] = "Uspesna registracija!";
		strcpy_s(client->client_data.name, regDes.name);

		printf("Registrovan klijent -> %s\n", regDes.name);

		// ZAKOMENTARISATI AKO PRAVI PROBLEM
		free(regDes.name);
		regDes.name = NULL;

		int iResult = SendPacket(client->client_data.acceptedSocket, potvrda, strlen(potvrda));
		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja podataka.\n");
		}
	}
}


void ForwardMessageThroughServer(char* serialized, NODE* head, NODE* client)
{
	char* yval = serialized;
	char* metadataName = yval + sizeof(int);
	char* ystrName = metadataName + sizeof(int);
	char* metadataText = ystrName + *(int*)metadataName;
	char* ystrText = metadataText + sizeof(int);
	MESSAGE_THROUGH_SERVER mtsDes;
	int sLenName = *((int*)metadataName);
	int sLenText = *((int*)metadataText);
	mtsDes.id = *((int*)yval);
	mtsDes.name = (char*)malloc((sLenName + 1) * sizeof(char));
	mtsDes.text = (char*)malloc((sLenText + 1) * sizeof(char));
	strncpy_s(mtsDes.name, sLenName + 1, ystrName, sLenName);
	strncpy_s(mtsDes.text, sLenText + 1, ystrText, sLenText);
	mtsDes.name[sLenName] = '\0';
	mtsDes.text[sLenText] = '\0';

	printf("\nIme tipa poruke: %s", mtsDes.name);
	printf("\nTekst poruke: %s", mtsDes.text);

	NODE* tempClientNode = head;
	while (tempClientNode != NULL) //prolazim kroz listu dok ne dodjem do kraja
	{
		if (strcmp(tempClientNode->client_data.name, client->client_data.name) != 0) 
		{
			int sNameLen = 0;
			int sTextLen = 0;
			int tLen = 0;
			char* serialized = 0;
			char* metadataName = 0;
			char* metadataText = 0;
			char* xval = 0;
			char* xstrName = 0;
			char* xstrText = 0;
			MESSAGE_THROUGH_SERVER mtsSer;
			mtsSer.id = 1;
			mtsSer.name = client->client_data.name;
			mtsSer.text = mtsDes.text;
			sNameLen = strlen(mtsSer.name);
			sTextLen = strlen(mtsSer.text);
			tLen = sizeof(int) + sNameLen + sTextLen;
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // +1 zbog free() errora
			xval = serialized;
			metadataName = serialized + sizeof(int);
			xstrName = metadataName + sizeof(int);
			metadataText = xstrName + sNameLen;
			xstrText = metadataText + sizeof(int);

			strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
			strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

			// OVO OBAVEZNO MORA DA BUDE ISPOD STRNCPY_S FUNKCIJA INACE NECE DA RADI LEPO
			*((int*)xval) = mtsSer.id;
			*((int*)metadataName) = sNameLen;
			*((int*)metadataText) = sTextLen;


			//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
			tLen += sizeof(int) + sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(tempClientNode->client_data.acceptedSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu registracione poruke serveru
			iResult = SendPacket(tempClientNode->client_data.acceptedSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}
		}
		
		tempClientNode = tempClientNode->next;
	}
	free(mtsDes.name);
	free(mtsDes.text);
}

void ForwardGameoverMessageThroughServer(char* serialized, NODE* head)
{
	char* yval = serialized;
	char* metadataName = yval + sizeof(int);
	char* ystrName = metadataName + sizeof(int);
	char* metadataText = ystrName + *(int*)metadataName;
	char* ystrText = metadataText + sizeof(int);
	MESSAGE_THROUGH_SERVER mtsDes;
	int sLenName = *((int*)metadataName);
	int sLenText = *((int*)metadataText);
	mtsDes.id = *((int*)yval);
	mtsDes.name = (char*)malloc((sLenName + 1) * sizeof(char));
	mtsDes.text = (char*)malloc((sLenText + 1) * sizeof(char));
	strncpy_s(mtsDes.name, sLenName + 1, ystrName, sLenName);
	strncpy_s(mtsDes.text, sLenText + 1, ystrText, sLenText);
	mtsDes.name[sLenName] = '\0';
	mtsDes.text[sLenText] = '\0';

	printf("\nIme tipa poruke: %s", mtsDes.name);
	printf("\nTekst poruke: %s", mtsDes.text);

	NODE* tempClient = head;
	while (tempClient != NULL) //prolazim kroz listu dok ne dodjem do kraja
	{
		int sNameLen = 0;
		int sTextLen = 0;
		int tLen = 0;
		char* serialized = 0;
		char* metadataName = 0;
		char* metadataText = 0;
		char* xval = 0;
		char* xstrName = 0;
		char* xstrText = 0;
		MESSAGE_THROUGH_SERVER mtsSer;
		mtsSer.id = 5;
		mtsSer.name = mtsDes.name;
		mtsSer.text = mtsDes.text;
		sNameLen = strlen(mtsSer.name);
		sTextLen = strlen(mtsSer.text);
		tLen = sizeof(int) + sNameLen + sTextLen;
		serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // +1 zbog free() errora
		xval = serialized;
		metadataName = serialized + sizeof(int);
		xstrName = metadataName + sizeof(int);
		metadataText = xstrName + sNameLen;
		xstrText = metadataText + sizeof(int);

		strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
		strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

		// OVO OBAVEZNO MORA DA BUDE ISPOD STRNCPY_S FUNKCIJA INACE NECE DA RADI LEPO
		*((int*)xval) = mtsSer.id;
		*((int*)metadataName) = sNameLen;
		*((int*)metadataText) = sTextLen;


		//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
		tLen += sizeof(int) + sizeof(int);

		// ovde saljem duzinu strukture koje saljem serveru
		int iResult = SendPacket(tempClient->client_data.acceptedSocket, (char*)(&tLen), 4);

		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja duzine.\n");
		}

		// ovde saljem citavu strukturu registracione poruke serveru
		iResult = SendPacket(tempClient->client_data.acceptedSocket, serialized, tLen);
		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja podataka.\n");
		}

		tempClient = tempClient->next;
	}

	free(mtsDes.name);
	free(mtsDes.text);
}


void RequestToBePlayer(char* serialized, NODE* head, NODE* client)
{
	char* yval = serialized;
	char* metadata = yval + sizeof(int);
	char* ystr = metadata + sizeof(int);
	MESSAGE_THROUGH_SERVER rdcDes;
	int sLen = *((int*)metadata);
	rdcDes.id = *((int*)yval);
	rdcDes.name = (char*)malloc((sLen + 1) * sizeof(char));
	strncpy_s(rdcDes.name, sLen + 1, ystr, sLen);
	rdcDes.name[sLen] = '\0';

	NODE* tempClientNode = head;
	while (tempClientNode != NULL) //prolazim kroz listu dok ne dodjem do kraja
	{
		// Ako vec postoji igrac, izvesti trazioca zahteva
		if (tempClientNode->client_data.isPlayer)
		{
			int sLen = 0;
			int tLen = 0;
			char* serialized = 0;
			char* metadata = 0;
			char* xval = 0;
			char* xstr = 0;
			MESSAGE_THROUGH_SERVER mtsSer;
			mtsSer.id = 4;
			mtsSer.name = tempClientNode->client_data.name;
			sLen = strlen(mtsSer.name);
			tLen = sizeof(int) + sLen;
			// + 1 zbog free() errora
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int)) + 1);
			xval = serialized;
			metadata = serialized + sizeof(int);
			xstr = metadata + sizeof(int);

			strncpy_s(xstr, sLen + 1, mtsSer.name, sLen);

			*((int*)metadata) = sLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 4 bajta za metadata polje koje sam dodao u memoriju
			tLen += sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(client->client_data.acceptedSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu registracione poruke serveru
			iResult = SendPacket(client->client_data.acceptedSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;

			free(rdcDes.name);
			rdcDes.name = NULL;
			return;
		}
		tempClientNode = tempClientNode->next;
	}

	// Ako ne postoji igrac, dozvoli posaljiocu zahteva da postane igrac i posalji mu poruku o uspehu
	client->client_data.isPlayer = true;

	sLen = 0;
	int tLen = 0;
	serialized = 0;
	metadata = 0;
	char* xval = 0;
	char* xstr = 0;
	MESSAGE_THROUGH_SERVER mtsSer;
	mtsSer.id = 2;
	mtsSer.name = client->client_data.name;
	sLen = strlen(mtsSer.name);
	tLen = sizeof(int) + sLen;
	// + 1 zbog free() errora
	serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int)) + 1);
	xval = serialized;
	metadata = serialized + sizeof(int);
	xstr = metadata + sizeof(int);

	strncpy_s(xstr, sLen + 1, mtsSer.name, sLen);

	*((int*)metadata) = sLen;
	*((int*)xval) = mtsSer.id;

	//dodajem jos 4 bajta za metadata polje koje sam dodao u memoriju
	tLen += sizeof(int);

	// ovde saljem duzinu strukture koje saljem serveru
	int iResult = SendPacket(client->client_data.acceptedSocket, (char*)(&tLen), 4);

	if (iResult == -1)
	{
		printf("Doslo je do greske prilikom slanja duzine.\n");
	}

	// ovde saljem citavu strukturu registracione poruke serveru
	iResult = SendPacket(client->client_data.acceptedSocket, serialized, tLen);
	if (iResult == -1)
	{
		printf("Doslo je do greske prilikom slanja podataka.\n");
	}

	free(rdcDes.name);
	rdcDes.name = NULL;
}


void AcceptInterval(char* serialized, NODE* head, NODE* client)
{
	char* yval = serialized;
	char* metadata = yval + sizeof(int);
	char* ystr = metadata + sizeof(int);
	MESSAGE_THROUGH_SERVER mtsDes;
	int sLen = *((int*)metadata);
	mtsDes.id = *((int*)yval);
	mtsDes.name = (char*)malloc((sLen + 1) * sizeof(char));
	strncpy_s(mtsDes.name, sLen + 1, ystr, sLen);
	mtsDes.name[sLen] = '\0';

	NODE* tempClientNode = head;
	while (tempClientNode != NULL) //prolazim kroz listu dok ne dodjem do kraja
	{
		// Ako je zahtev nastao od igraca, posalji poruku o prihvatanju intervala
		if (tempClientNode->client_data.isPlayer && !strcmp(tempClientNode->client_data.name,client->client_data.name))
		{
			int sLen = 0;
			int tLen = 0;
			char* serialized = 0;
			char* metadata = 0;
			char* xval = 0;
			char* xstr = 0;
			MESSAGE_THROUGH_SERVER mtsSer;
			mtsSer.id = 3;
			mtsSer.name = tempClientNode->client_data.name;
			sLen = strlen(mtsSer.name);
			tLen = sizeof(int) + sLen;
			// + 1 zbog free() errora
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int)) + 1);
			xval = serialized;
			metadata = serialized + sizeof(int);
			xstr = metadata + sizeof(int);

			strncpy_s(xstr, sLen + 1, mtsSer.name, sLen);

			*((int*)metadata) = sLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 4 bajta za metadata polje koje sam dodao u memoriju
			tLen += sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(client->client_data.acceptedSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu registracione poruke serveru
			iResult = SendPacket(client->client_data.acceptedSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;

			free(mtsDes.name);
			mtsDes.name = NULL;
			return;
		}
		tempClientNode = tempClientNode->next;
	}

}