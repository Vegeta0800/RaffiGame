
#pragma once
//EXTERNAL INCLUDES
#include <cstdio>
#include <string>
//INTERNAL INCLUDES

//Message struct that every message inherits
#pragma pack(push, 1)
struct Message
{
	//Identifier
	uint8_t type;
};
#pragma pack(pop)

//Query message for login
#pragma pack(push, 1)
struct Query : public Message
{
	std::string name;
	std::string password;
};
#pragma pack(pop)

//Query message response
#pragma pack(push, 1)
struct QueryResponse : public Message
{
	bool success;
};
#pragma pack(pop)

//Message types that get send over network
enum class MessageType
{
	QUERY = 0,
	CREATEROOM = 1,
	JOINROOM = 2,
	LEAVEROOM = 3,
	READY = 4,
	START = 5,
	QUERYRESP = 6,
	CREATEROOMRESP = 7,
	JOINROOMRESP = 8,
	LEAVEROOMRESP = 9,
	READYRESP = 10
};


