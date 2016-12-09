#include "handler.h"
#include "database.h"
#include <iostream>
#include <openssl/sha.h>


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
		if (!request["connect"].empty())
		{
			std::string privateId = request["connect"]["privateId"].asString();
			std::string publicId = GetPublicIdFromPrivateId(privateId);

			mpDatabase->ConnectClient(privateId, publicId, mClientAddress);

			reply["reply"]["publicId"] = publicId;
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
		}

		// Heartbeat request : write the new client data in the database
		if (!request["heartbeat"].empty())
		{
			std::string privateId = request["heartbeat"]["privateId"].asString();

			if (mpDatabase->IsConnectedPrivate(privateId))
			{
				ClientData data;
				for (std::string& key : request["heartbeat"]["data"].getMemberNames())
				{
					SetClientAttribute(data.attributes[key], request["heartbeat"]["data"].get(key, defValue));
				}

				mpDatabase->UpdateClient(privateId, data);
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
				const ClientData& data = mpDatabase->QueryClient(targetId);
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
		if (!request["search"].empty())
		{
			ClientAttribute value;
			std::string key = request["search"]["key"].asString();
			SetClientAttribute(value, request["search"]["value"]);
			SearchCriteriaType criteria = GetCriteria(request["search"]["criteria"].asString());

			ClientSearchResult results = mpDatabase->SearchClients(key, value, criteria);
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

std::string Handler::GetPublicIdFromPrivateId(const std::string privateId)
{
	uint8_t hash[SHA512_DIGEST_LENGTH];
	SHA512((const unsigned char*)privateId.c_str(), privateId.length(), hash);

	char hashString[2 * SHA512_DIGEST_LENGTH + 1];
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		snprintf(hashString + i * 2, 3, "%02x", hash[i]);
	}

	return std::string(hashString);
}

SearchCriteriaType Handler::GetCriteria(const std::string& v)
{
	if (v == "<")
		return SearchCriteriaType::T_LESSER;
	else if (v == ">")
		return SearchCriteriaType::T_GREATER;
	else if (v == "<=")
		return SearchCriteriaType::T_LESSER_EQ;
	else if (v == ">=")
		return SearchCriteriaType::T_GREATER_EQ;
	else
		return SearchCriteriaType::T_EQUAL;
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