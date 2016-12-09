#pragma once

#include <string>
#include <memory>
#include "json/json.h"
#include "database.h"


/*-----------------------------------------------------------------------------
	Handler class definition
-----------------------------------------------------------------------------*/

class Handler
{
public:

	Handler(std::shared_ptr<Database> db, std::string clientAddress);

	~Handler();


public:

	// Process data from a request and write a reply. Return true to keep connection.
	bool ProcessClientRequest(const std::string& dataIn, std::string& dataOut);


private:

	// Generate a safe public identifier from the private identifier that is never revealed
	static std::string GetPublicIdFromPrivateId(const std::string privateId);

	// Get a search criteria from string
	static SearchCriteriaType GetCriteria(const std::string& v);

	// Set a client attribute from a JSON value
	static void SetClientAttribute(ClientAttribute& a, const Json::Value& v);

	// Set a JSON value from a client attribute
	static void SetJsonValue(Json::Value& v, const ClientAttribute& a);


private:

	std::shared_ptr<Database>                       mpDatabase;
	Json::Reader                                    mReader;
	std::string                                     mClientAddress;


};
