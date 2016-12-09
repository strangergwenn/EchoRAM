#include "database.h"
#include <iostream>
#include <cassert>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

Database::Database(int updatePeriod, int clientIdleTime)
{
	mStartupTime = std::chrono::system_clock::now();
	mUpdatePeriod = updatePeriod;
	mClientIdleTime = clientIdleTime;
	mRunning.store(true);

	mThread = std::thread(&Database::BackgroundRefresh, this);
}

Database::~Database()
{
	mRunning.store(false);

	mThread.join();
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
	DatabaseTime now = std::chrono::system_clock::now();
	auto diff = (now - mStartupTime);
	return std::chrono::duration_cast<std::chrono::seconds>(diff);
}

void Database::ConnectClient(const std::string& privateId, const std::string& publicId, const std::string& clientAddress)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mPrivateToPublic[privateId] = publicId;
	mData[publicId] = ClientData(privateId, clientAddress);

	assert(mData[publicId].privateId.length());
}

void Database::DisconnectClient(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mData.erase(mData.find(mPrivateToPublic[privateId]));
	mPrivateToPublic.erase(mPrivateToPublic.find(privateId));
}

void Database::HeartbeatClient(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mData[mPrivateToPublic[privateId]].lastUpdateTime = std::chrono::system_clock::now();
}

void Database::UpdateClient(const std::string& privateId, const ClientData& data)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mData[mPrivateToPublic[privateId]] = data;
	mData[mPrivateToPublic[privateId]].lastUpdateTime = std::chrono::system_clock::now();
}

const ClientData& Database::QueryClientPublic(const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return mData[publicId];
}

const ClientData& Database::QueryClientPrivate(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	return mData[mPrivateToPublic[privateId]];
}

ClientSearchResult Database::SearchClients(const std::vector<ClientSearchCriterion>& criteria, int maxCount)
{
	std::lock_guard<std::mutex> lock(mMutex);
	ClientSearchResult result;
	int count = 0;

	for (auto& client : mData)
	{
		bool match = true;

		// Get client data
		for (auto& crit : criteria)
		{
			// Client doesn't have that attribute
			if (client.second.attributes.find(crit.key) == client.second.attributes.end())
			{
				match = false;
				break;
			}

			// Client has attribute, check if it matches
			else
			{
				const ClientAttribute& attr = client.second.attributes[crit.key];
				switch (crit.condition)
				{
					case ClientSearchCondition::T_EQUAL:      match = match && (attr == crit.value); break;
					case ClientSearchCondition::T_NEQUAL:     match = match && (attr != crit.value); break;
					case ClientSearchCondition::T_LESSER:     match = match && (attr <  crit.value); break;
					case ClientSearchCondition::T_GREATER:    match = match && (attr >  crit.value); break;
					case ClientSearchCondition::T_LESSER_EQ:  match = match && (attr <= crit.value); break;
					case ClientSearchCondition::T_GREATER_EQ: match = match && (attr >= crit.value); break;
				}
			}
		}

		// Result matches !
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

	return result;
}


/*-----------------------------------------------------------------------------
	Background process
-----------------------------------------------------------------------------*/

void Database::BackgroundRefresh()
{
	while (mRunning.load())
	{
		// Every second, update the database
		std::this_thread::sleep_for(std::chrono::seconds(mUpdatePeriod));
		std::lock_guard<std::mutex> lock(mMutex);
		DatabaseTime now = std::chrono::system_clock::now();

		// Detect idle clients
		std::vector<std::string> idleClients;
		for (auto& client : mData)
		{
			auto diff = (now - client.second.lastUpdateTime);
			if (std::chrono::duration_cast<std::chrono::seconds>(diff).count() > mClientIdleTime)
			{
				idleClients.push_back(client.second.privateId);
			}
		}

		// Disconnect idle clients
		for (auto& privateId : idleClients)
		{
			mData.erase(mData.find(mPrivateToPublic[privateId]));
			mPrivateToPublic.erase(mPrivateToPublic.find(privateId));
		}

	}
}
