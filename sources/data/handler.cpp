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

std::string Handler::GetPublicIdFromPrivateId(const std::string privateId)
{
	uint8_t hash[SHA512_DIGEST_LENGTH];
	SHA512((const unsigned char*)privateId.c_str(), privateId.length() - 1, hash);

	char hashString[2 * SHA512_DIGEST_LENGTH + 1];
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		snprintf(hashString + i * 2, 3, "%02x", hash[i]);
	}

	return std::string(hashString);
}

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

			mpDatabase->ConnectClient(privateId, publicId);

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

		// Heartbeat request : write the new client data in the database
		if (!request["heartbeat"].empty())
		{
			std::string privateId = request["heartbeat"]["privateId"].asString();

			if (mpDatabase->IsConnectedPrivate(privateId))
			{
				request["heartbeat"]["data"]["clientAddress"] = mClientAddress;

				ClientData data;
				for (std::string& key : request["heartbeat"]["data"].getMemberNames())
				{
					data[key] = request["heartbeat"]["data"].get(key, defValue).asString();
				}
				data["clientAddress"] = mClientAddress;

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
				ClientData data = mpDatabase->QueryClient(targetId);
				for (auto& entry : data)
				{
					reply["reply"]["data"][entry.first] = entry.second;
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
			std::string key = request["search"]["key"].asString();
			std::string value = request["search"]["value"].asString();

			ClientSearchResult results = mpDatabase->SearchClients(key, value);
			for (auto& result : results)
			{
				reply["reply"]["clients"][result.first] = result.second;
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

