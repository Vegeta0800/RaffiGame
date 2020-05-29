
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
