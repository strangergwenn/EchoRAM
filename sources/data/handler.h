#pragma once

#include <string>
#include <memory>
#include "json/json.h"

class Database;


/*-----------------------------------------------------------------------------
	Handler class definition
-----------------------------------------------------------------------------*/

class Handler
{
public:

	Handler(std::shared_ptr<Database> db, std::string clientAddress);

	~Handler();

public:

	// Generate a safe public identifier from the private identifier that is never revealed
	static std::string GetPublicIdFromPrivateId(const std::string privateId);

	// Process data from a request and write a reply. Return true to keep connection.
	bool ProcessClientRequest(const std::string& dataIn, std::string& dataOut);


private:

	std::shared_ptr<Database>                       mpDatabase;
	Json::Reader                                    mReader;
	std::string                                     mClientAddress;


};
