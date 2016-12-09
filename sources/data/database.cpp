#include "database.h"
#include <iostream>
#include <cassert>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

Database::Database()
{
	mStartupTime = std::chrono::high_resolution_clock::now();
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

std::chrono::seconds Database::GetUptime() const
{
	auto now = std::chrono::high_resolution_clock::now();
	auto diff = (now - mStartupTime);
	return std::chrono::duration_cast<std::chrono::seconds>(diff);
}

void Database::ConnectClient(const std::string& privateId, const std::string& publicId, const std::string& clientAddress)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mPrivateToPublic[privateId] = publicId;
	mData[publicId] = ClientData(privateId, clientAddress);
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

	for (auto& entry : data.attributes)
	{
		assert(entry.second.type != ClientAttributeType::T_NONE);
	}
	mData[mPrivateToPublic[privateId]] = data;
	mData[mPrivateToPublic[privateId]].lastUpdateTime = std::chrono::high_resolution_clock::now();
}

const ClientData& Database::QueryClient(const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return mData[publicId];
}

ClientSearchResult Database::SearchClients(const std::string& key, const ClientAttribute& value, SearchCriteriaType criteria, int maxCount)
{
	std::lock_guard<std::mutex> lock(mMutex);
	ClientSearchResult result;
	int count = 0;

	for (auto& client : mData)
	{
		// Get client data
		if (client.second.attributes.find(key) != client.second.attributes.end())
		{
			const ClientAttribute& attr = client.second.attributes[key];

			// Does it match ?
			bool match = false;
			switch (criteria)
			{
				case SearchCriteriaType::T_EQUAL:      match = (attr == value); break;
				case SearchCriteriaType::T_LESSER:     match = (attr <  value); break;
				case SearchCriteriaType::T_GREATER:    match = (attr >  value); break;
				case SearchCriteriaType::T_LESSER_EQ:  match = (attr <= value); break;
				case SearchCriteriaType::T_GREATER_EQ: match = (attr >= value); break;
			}

			// Result matches
			if (match)
			{
				result[client.first] = client.second;
				count++;
			}

			// Limit
			if (count >= maxCount)
			{
				break;
			}
		}
	}

	return result;
}
