#include "../TCPLib/TCPCommunication.h"
#include "../Common/Header.h"
#include "../ListLib/ListMethods.h"
#include "../ClientFunctions/ClientMethods.h"


bool TryRegistration(SOCKET connectSocket, char* messageToSend)
{
	unsigned int len_max = 128;
	unsigned int current_size = 0;

	char *pStr = (char *)malloc(len_max);
	current_size = len_max;
	bool validRegistration = false;

	printf("Unesite naziv za registraciju: ");

	if (pStr != NULL)
	{
		int c = EOF;
		unsigned int i = 0;
		//accept user input until hit enter or end of file
		while ((c = getchar()) != '\n' && c != EOF)
		{
			pStr[i++] = (char)c;

			//if i reached maximize size then realloc size
			if (i == current_size)
			{
				current_size = i + len_max;
				pStr = (char *)realloc(pStr, current_size);
			}
		}

		pStr[i] = '\0';

		switch (pStr[0])
		{
			case '\0':
			{
				printf("Nevalidan naziv!\n");
				break;
			}
			case '\t':
			{
				printf("Nevalidan naziv!\n");
				break;
			}
			case ' ':
			{
				printf("Nevalidan naziv!\n");
				break;
			}
			default:
			{
				validRegistration = SendRegistration(pStr, connectSocket, messageToSend);
				break;
			}
		}

		free(pStr);
		pStr = NULL;
	}

	return validRegistration;
}

bool SendRegistration(char* pStr, SOCKET connectSocket, char* messageToSend)
{
	int sLen = 0;
	int tLen = 0;
	char* serialized = 0;
	char* metadata = 0;
	char* xval = 0;
	char* xstr = 0;
	REGISTRATION regSer;
	regSer.id = 0;
	regSer.name = pStr;
	sLen = strlen(regSer.name); 
	tLen = sizeof(int) + sLen; 
	serialized = (char *)malloc(sizeof(char) * (tLen + sizeof(int)) + 1); 
	xval = serialized;
	metadata = serialized + sizeof(int);
	xstr = metadata + sizeof(int);
	strncpy_s(xstr, sLen + 1, regSer.name, sLen); 
	*((int*)metadata) = sLen; 
	*((int*)xval) = regSer.id; 

	//dodajem jos 4 bajta za metadata polje koje sam dodao u memoriju
	tLen += sizeof(int);

	// ovde saljem duzinu strukture koje saljem serveru
	int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

	if (iResult == -1)
	{
		printf("Doslo je do greske prilikom slanja duzine.\n");
		//return 0;
	}

	// ovde saljem citavu strukturu registracione poruke serveru
	iResult = SendPacket(connectSocket, serialized, tLen);
	if (iResult == -1)
	{
		printf("Doslo je do greske prilikom slanja podataka.\n");
	}

	free(serialized);
	serialized = NULL;

	iResult = RecievePacket(connectSocket, messageToSend, 21);
	if (iResult == -1)
	{
		printf("Greska prilikom prijema poruke");
	}

	messageToSend[21] = '\0';
	printf("%s\n", messageToSend);

	if (messageToSend[0] == 'U')
		return true;
	else
		return false;
}

void ChooseTypeOfMessageToSend(SOCKET connectSocket)
{
	unsigned int len_max = 128;
	unsigned int current_size = 0;

	char* pStr = (char*)malloc(len_max);
	current_size = len_max;

	printf("\nUnesite p da zatrazite da budete igrac ili i da igrate pogadjanje broja: ");

	if (pStr != NULL)
	{
		int c = EOF;
		unsigned int i = 0;
		//accept user input until hit enter or end of file
		while ((c = getchar()) != '\n' && c != EOF)
		{
			pStr[i++] = (char)c;

			//if i reached maximize size then realloc size
			if (i == current_size)
			{
				current_size = i + len_max;
				pStr = (char*)realloc(pStr, current_size);
			}
		}

		pStr[i] = '\0';

		if (pStr[0] == 'p')
		{
			len_max = 128;
			current_size = 0;
			current_size = len_max;

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
			mtsSer.id = 2;
			mtsSer.name = const_cast<char *>("options");
			mtsSer.text = const_cast<char*>("p");
			sNameLen = strlen(mtsSer.name);
			sTextLen = strlen(mtsSer.text);
			tLen = sizeof(int) + sNameLen + sTextLen;
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
			xval = serialized;
			metadataName = serialized + sizeof(int);
			xstrName = metadataName + sizeof(int);
			metadataText = xstrName + sNameLen;
			xstrText = metadataText + sizeof(int);

			strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
			strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

			*((int*)metadataName) = sNameLen;
			*((int*)metadataText) = sTextLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
			tLen += sizeof(int) + sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu poruke preko servera
			iResult = SendPacket(connectSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;
		}
		else if (pStr[0] == 'i')
		{
			printf("Slanje poruke preko servera!");

			len_max = 128;
			current_size = 0;

			char* pStr2 = (char*)malloc(len_max);
			current_size = len_max;

			printf("\nUnesite broj za koji mislite da je tacan: ");

			if (pStr2 != NULL)
			{
				int c = EOF;
				unsigned int i = 0;
				//accept user input until hit enter or end of file
				while ((c = getchar()) != '\n' && c != EOF)
				{
					pStr2[i++] = (char)c;

					//if i reached maximize size then realloc size
					if (i == current_size)
					{
						current_size = i + len_max;
						pStr2 = (char*)realloc(pStr2, current_size);
					}

				}
				pStr2[i] = '\0';

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
				mtsSer.name = const_cast<char *>("gameplay");
				mtsSer.text = pStr2;
				sNameLen = strlen(mtsSer.name);
				sTextLen = strlen(mtsSer.text);
				tLen = sizeof(int) + sNameLen + sTextLen;
				serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
				xval = serialized;
				metadataName = serialized + sizeof(int);
				xstrName = metadataName + sizeof(int);
				metadataText = xstrName + sNameLen;
				xstrText = metadataText + sizeof(int);

				strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
				strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

				*((int*)metadataName) = sNameLen;
				*((int*)metadataText) = sTextLen;
				*((int*)xval) = mtsSer.id;

				//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
				tLen += sizeof(int) + sizeof(int);

				// ovde saljem duzinu strukture koje saljem serveru
				int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

				if (iResult == -1)
				{
					printf("Doslo je do greske prilikom slanja duzine.\n");
				}

				// ovde saljem citavu strukturu poruke preko servera
				iResult = SendPacket(connectSocket, serialized, tLen);
				if (iResult == -1)
				{
					printf("Doslo je do greske prilikom slanja podataka.\n");
				}

				// ZAKOMENTARISATI AKO PRAVI PROBLEM
				free(serialized);
				serialized = NULL;


				free(pStr2);
				pStr2 = NULL;
			}
		}
		else {
			printf("Nevalidna opcija!\n");
			free(pStr);
			pStr = NULL;
			return;
		}

		free(pStr);
		pStr = NULL;
	}
}

void ChooseTypeOfMessageToSendForPlayer (SOCKET connectSocket)
{
	unsigned int len_max = 128;
	unsigned int current_size = 0;

	char* pStr = (char*)malloc(len_max);
	current_size = len_max;

	printf("\nUnesite m za manje, v za vece ili t za tacan broj: ");

	if (pStr != NULL)
	{
		int c = EOF;
		unsigned int i = 0;
		//accept user input until hit enter or end of file
		while ((c = getchar()) != '\n' && c != EOF)
		{
			pStr[i++] = (char)c;

			//if i reached maximize size then realloc size
			if (i == current_size)
			{
				current_size = i + len_max;
				pStr = (char*)realloc(pStr, current_size);
			}
		}

		pStr[i] = '\0';

		if (pStr[0] == 't')
		{
			len_max = 128;
			current_size = 0;
			current_size = len_max;

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
			mtsSer.name = const_cast<char*>("gameover");
			mtsSer.text = const_cast<char*>("win");
			sNameLen = strlen(mtsSer.name);
			sTextLen = strlen(mtsSer.text);
			tLen = sizeof(int) + sNameLen + sTextLen;
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
			xval = serialized;
			metadataName = serialized + sizeof(int);
			xstrName = metadataName + sizeof(int);
			metadataText = xstrName + sNameLen;
			xstrText = metadataText + sizeof(int);

			strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
			strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

			*((int*)metadataName) = sNameLen;
			*((int*)metadataText) = sTextLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
			tLen += sizeof(int) + sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu poruke preko servera
			iResult = SendPacket(connectSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;
		}
		else if (pStr[0] == 'm')
		{
			len_max = 128;
			current_size = 0;
			current_size = len_max;

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
			mtsSer.name = const_cast<char*>("gameplay");
			mtsSer.text = const_cast<char*>("manje");
			sNameLen = strlen(mtsSer.name);
			sTextLen = strlen(mtsSer.text);
			tLen = sizeof(int) + sNameLen + sTextLen;
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
			xval = serialized;
			metadataName = serialized + sizeof(int);
			xstrName = metadataName + sizeof(int);
			metadataText = xstrName + sNameLen;
			xstrText = metadataText + sizeof(int);

			strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
			strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

			*((int*)metadataName) = sNameLen;
			*((int*)metadataText) = sTextLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
			tLen += sizeof(int) + sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu poruke preko servera
			iResult = SendPacket(connectSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;
		}
		else if (pStr[0] == 'v')
		{
			len_max = 128;
			current_size = 0;
			current_size = len_max;

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
			mtsSer.name = const_cast<char*>("gameplay");
			mtsSer.text = const_cast<char*>("vece");
			sNameLen = strlen(mtsSer.name);
			sTextLen = strlen(mtsSer.text);
			tLen = sizeof(int) + sNameLen + sTextLen;
			serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
			xval = serialized;
			metadataName = serialized + sizeof(int);
			xstrName = metadataName + sizeof(int);
			metadataText = xstrName + sNameLen;
			xstrText = metadataText + sizeof(int);

			strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
			strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

			*((int*)metadataName) = sNameLen;
			*((int*)metadataText) = sTextLen;
			*((int*)xval) = mtsSer.id;

			//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
			tLen += sizeof(int) + sizeof(int);

			// ovde saljem duzinu strukture koje saljem serveru
			int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja duzine.\n");
			}

			// ovde saljem citavu strukturu poruke preko servera
			iResult = SendPacket(connectSocket, serialized, tLen);
			if (iResult == -1)
			{
				printf("Doslo je do greske prilikom slanja podataka.\n");
			}

			// ZAKOMENTARISATI AKO PRAVI PROBLEM
			free(serialized);
			serialized = NULL;
		}
		else {
			printf("Nevalidna opcija!\n");
			free(pStr);
			pStr = NULL;
			return;
		}

		free(pStr);
		pStr = NULL;
	}
}

void SendIntervalToServer(SOCKET connectSocket)
{
	unsigned int len_max = 128;
	unsigned int current_size = 0;

	char* pStr = (char*)malloc(len_max);
	current_size = len_max;

	printf("\nUnesite interval u kom se nalazi zamiljeni broj: ");

	if (pStr != NULL)
	{
		int c = EOF;
		unsigned int i = 0;
		//accept user input until hit enter or end of file
		while ((c = getchar()) != '\n' && c != EOF)
		{
			pStr[i++] = (char)c;

			//if i reached maximize size then realloc size
			if (i == current_size)
			{
				current_size = i + len_max;
				pStr = (char*)realloc(pStr, current_size);
			}
		}

		pStr[i] = '\0';

		switch (pStr[0])
		{
			case '\0':
			{
				printf("Nevalidna opcija!\n");
				free(pStr);
				pStr = NULL;
				return;
			}
			case '\t':
			{
				printf("Nevalidna opcija!\n");
				free(pStr);
				pStr = NULL;
				return;
			}
			case ' ':
			{
				printf("Nevalidna opcija!\n");
				free(pStr);
				pStr = NULL;
				return;
			}
		}

		len_max = 128;
		current_size = 0;
		current_size = len_max;

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
		mtsSer.id = 3;
		mtsSer.name = const_cast<char*>("interval");
		mtsSer.text = pStr;
		sNameLen = strlen(mtsSer.name);
		sTextLen = strlen(mtsSer.text);
		tLen = sizeof(int) + sNameLen + sTextLen;
		serialized = (char*)malloc(sizeof(char) * (tLen + sizeof(int) + sizeof(int)) + 1); // + 1 jer free bez toga izbacuje error
		xval = serialized;
		metadataName = serialized + sizeof(int);
		xstrName = metadataName + sizeof(int);
		metadataText = xstrName + sNameLen;
		xstrText = metadataText + sizeof(int);

		strncpy_s(xstrName, sNameLen + 1, mtsSer.name, sNameLen);
		strncpy_s(xstrText, sTextLen + 1, mtsSer.text, sTextLen);

		*((int*)metadataName) = sNameLen;
		*((int*)metadataText) = sTextLen;
		*((int*)xval) = mtsSer.id;

		//dodajem jos 8 bajta za metadataName i metadataText polja koja sam dodao u memoriju
		tLen += sizeof(int) + sizeof(int);

		// ovde saljem duzinu strukture koje saljem serveru
		int iResult = SendPacket(connectSocket, (char*)(&tLen), 4);

		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja duzine.\n");
		}

		// ovde saljem citavu strukturu poruke preko servera
		iResult = SendPacket(connectSocket, serialized, tLen);
		if (iResult == -1)
		{
			printf("Doslo je do greske prilikom slanja podataka.\n");
		}

		free(serialized);
		serialized = NULL;

		free(pStr);
		pStr = NULL;
	}
}

void ReceiveMessageThroughServer(char* serialized)
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

	printf("\nPoruka od %s: %s\n", mtsDes.name, mtsDes.text);

	free(mtsDes.name);
	mtsDes.name = NULL;

	free(mtsDes.text);
	mtsDes.text = NULL;
}