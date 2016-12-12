#include "handler.h"
#include <iostream>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

Handler::Handler(std::shared_ptr<Database> pDb, std::string clientAddress)
	: mpDatabase(pDb)
	, mClientAddress(clientAddress)
{
}

Handler::~Handler()
{
}


/*-----------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------*/

bool Handler::ProcessClientRequest(const std::string& dataIn, std::string& dataOut)
{
	Json::Value request;
	Json::Value reply;
	Json::Value defValue;
	bool isSuccess = true;

	if (mReader.parse(dataIn, request))
	{
		reply["reply"]["status"] = std::string("OK");

		// Connection request : add / update entry in database
		if (!request["connect"].empty() && request["connect"]["privateId"].asString().length())
		{
			std::string privateId = request["connect"]["privateId"].asString();
			std::string publicId = request["connect"]["publicId"].asString();

			mpDatabase->ConnectClient(privateId, publicId, mClientAddress);
		}

		// Connection request : add / update entry in database
		if (!request["disconnect"].empty())
		{
			std::string privateId = request["disconnect"]["privateId"].asString();

			if (mpDatabase->IsConnectedPrivate(privateId))
			{
				mpDatabase->DisconnectClient(privateId);
			}
			else
			{
				reply["reply"]["status"] = std::string("Target is not connected");
			}
		}

		// Server stats
		if (!request["stats"].empty())
		{
			reply["reply"]["count"] = mpDatabase->GetConnectedClientsCount();
			reply["reply"]["uptime"] = mpDatabase->GetUptime().count();
		}

		// Update request : write the new client data in the database
		if (!request["update"].empty())
		{
			std::string privateId = request["update"]["privateId"].asString();

			if (mpDatabase->IsConnectedPrivate(privateId))
			{
				ClientData data = mpDatabase->QueryClientPrivate(privateId);
				for (std::string& key : request["update"]["data"].getMemberNames())
				{
					SetClientAttribute(data.attributes[key], request["update"]["data"].get(key, defValue));
				}

				mpDatabase->UpdateClient(privateId, data);
			}
			else
			{
				reply["reply"]["status"] = std::string("Target is not connected");
			}
		}

		// Heartbeat request : mark client as active
		if (!request["heartbeat"].empty())
		{
			std::string privateId = request["heartbeat"]["privateId"].asString();

			if (mpDatabase->IsConnectedPrivate(privateId))
			{
				mpDatabase->HeartbeatClient(privateId);
			}
			else
			{
				reply["reply"]["status"] = std::string("Target is not connected");
			}
		}

		// Query client info
		if (!request["query"].empty())
		{
			std::string targetId = request["query"]["targetId"].asString();

			if (mpDatabase->IsConnectedPublic(targetId))
			{
				const ClientData& data = mpDatabase->QueryClientPublic(targetId);
				for (auto& entry : data.attributes)
				{
					SetJsonValue(reply["reply"]["data"][entry.first], entry.second);
				}
			}
			else
			{
				reply["reply"]["status"] = std::string("Target is not connected");
			}
		}

		// Search clients
		if (!request["search"].empty() && request["search"].isArray())
		{
			// Process the search parameters
			std::vector<ClientSearchCriterion> criteria;
			for (auto& searchCriterion : request["search"])
			{
				ClientAttribute value;
				std::string key = searchCriterion["key"].asString();
				SetClientAttribute(value, searchCriterion["value"]);
				ClientSearchCondition type = GetCondition(searchCriterion["condition"].asString());

				criteria.push_back(ClientSearchCriterion(key, value, type));
			}

			// Process results
			ClientSearchResult results = mpDatabase->SearchClients(criteria);
			for (auto& data : results)
			{
				for (auto& entry : data.second.attributes)
				{
					SetJsonValue(reply["reply"]["clients"][data.first][entry.first], entry.second);
				}
			}
		}
	}

	// Disconnect
	else
	{
		reply["reply"]["status"] = std::string("Could not parse request");
		isSuccess = false;
	}

	// Send reply
	Json::StreamWriterBuilder builder;
	dataOut = Json::writeString(builder, reply);
	return isSuccess;
}


/*-----------------------------------------------------------------------------
	Private methods
-----------------------------------------------------------------------------*/

ClientSearchCondition Handler::GetCondition(const std::string& v)
{
	if (v == "<")
		return ClientSearchCondition::T_LESSER;
	else if (v == ">")
		return ClientSearchCondition::T_GREATER;
	else if (v == "<=")
		return ClientSearchCondition::T_LESSER_EQ;
	else if (v == ">=")
		return ClientSearchCondition::T_GREATER_EQ;
	else
		return ClientSearchCondition::T_EQUAL;
}

void Handler::SetClientAttribute(ClientAttribute& a, const Json::Value& v)
{
	if (v.isInt())
		a = ClientAttribute(v.asInt());
	else if (v.isUInt())
		a = ClientAttribute(v.asUInt());
	else if (v.isDouble())
		a = ClientAttribute(v.asDouble());
	else if (v.isBool())
		a = ClientAttribute(v.asBool());
	else
		a = ClientAttribute(v.asString());
}

void Handler::SetJsonValue(Json::Value& v, const ClientAttribute& a)
{
	switch (a.type)
	{
		case ClientAttributeType::T_STR: v = a.s; break;
		case ClientAttributeType::T_INT: v = a.i; break;
		case ClientAttributeType::T_UNS: v = a.u; break;
		case ClientAttributeType::T_DBL: v = a.d; break;
		case ClientAttributeType::T_BOL: v = a.b; break;
	}
}