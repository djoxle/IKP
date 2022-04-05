#pragma once


/*
	id   -> 0 - message for registration
	name -> client name for registration
*/
typedef struct registration_st
{
	int id;
	char* name;
}REGISTRATION;


/*
	id   -> 1 - communication through server
	id   -> 2 - be a player request
	id   -> 3 - send interval request
	id   -> 4 - player already assigned error
	id   -> 5 - gameover message
	name -> client name or message type
	text -> text of the message
*/
typedef struct message_through_server_st
{
	int id; 
	char* name;    
	char* text;		
} MESSAGE_THROUGH_SERVER;

/*
	session data for each client
	isRegistered   -> true - user is successfully registered
	isPlayer   -> true - user has player privilegies
	isGameOver   -> true - other players successfully found the number
*/
typedef struct client_session_data_st
{
	bool isRegistered;
	bool isPlayer;
	bool isGameOver;
} CLIENT_SESSION_DATA;


