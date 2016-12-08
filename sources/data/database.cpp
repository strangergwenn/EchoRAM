#include "database.h"
#include <iostream>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

Database::Database()
{
}

Database::~Database()
{
}


/*-----------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------*/

bool Database::IsConnectedPublic(const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return (mData.find(publicId) != mData.end());
}

bool Database::IsConnectedPrivate(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return (mPrivateToPublic.find(privateId) != mPrivateToPublic.end());
}

int Database::GetConnectedClientsCount() const
{
	return static_cast<int>(mData.size());
}

void Database::ConnectClient(const std::string& privateId, const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mPrivateToPublic[privateId] = publicId;
	mData[publicId] = ClientData(privateId);
}

void Database::DisconnectClient(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mData.erase(mData.find(mPrivateToPublic[privateId]));
	mPrivateToPublic.erase(mPrivateToPublic.find(privateId));
}

void Database::UpdateClient(const std::string& privateId, const ClientData& data)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mData[mPrivateToPublic[privateId]] = data;
}

const ClientData& Database::QueryClient(const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return mData[publicId];
}

ClientSearchResult Database::SearchClients(const std::string& key, const ClientAttribute& value, int maxCount)
{
	std::lock_guard<std::mutex> lock(mMutex);

	ClientSearchResult result;
	int count = 0;

	for (auto& client : mData)
	{
		// Look for value
		ClientData& clientData = client.second;
		if (clientData.attributes[key] == value)
		{
			result[client.first] = clientData.attributes[key];
			count++;
		}

		// Limit
		if (count >= maxCount)
		{
			break;
		}
	}

	return result;
}
