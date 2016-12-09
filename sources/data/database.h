#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include "clientattribute.h"


/*-----------------------------------------------------------------------------
	Database types
-----------------------------------------------------------------------------*/

// Time
using DatabaseTime = std::chrono::time_point<std::chrono::system_clock>;


// Map of client attributes
class ClientData
{
public:

	ClientData()
	{
		lastUpdateTime = std::chrono::system_clock::now();
	}

	ClientData(const std::string& id, const std::string& addr)
		: privateId(id)
		, clientAddress(addr)
	{
		lastUpdateTime = std::chrono::system_clock::now();
	}

public:

	std::string                                     privateId;
	std::string                                     clientAddress;
	std::map<std::string, ClientAttribute>          attributes;
	DatabaseTime                                    lastUpdateTime;

};


// Search criteria
enum class ClientSearchCondition { T_EQUAL = 0, T_NEQUAL, T_LESSER, T_GREATER, T_LESSER_EQ, T_GREATER_EQ };

// Criteria for a search
class ClientSearchCriterion
{
public:

	ClientSearchCriterion(const std::string& k, const ClientAttribute& v, ClientSearchCondition c)
		: key(k)
		, value(v)
		, condition(c)
	{}

public:

	std::string                                     key;
	ClientAttribute                                 value;
	ClientSearchCondition                           condition;

};

// Search result for a value
using ClientSearchResult = std::map<std::string, ClientData>;


/*-----------------------------------------------------------------------------
	Database class definition
-----------------------------------------------------------------------------*/

class Database
{
public:

	Database(int updatePeriod, int clientIdleTime);

	~Database();

public:

	// Check if this client is already connected (exists in the database)
	bool IsConnectedPublic(const std::string& publicId);

	// Check if this client is already connected (exists in the database)
	bool IsConnectedPrivate(const std::string& privateId);

	// Check how many clients are connected
	int GetConnectedClientsCount() const;

	// How much time have we been running
	std::chrono::seconds GetUptime() const;


	// Connect this client, adding the public + private IDs in database
	void ConnectClient(const std::string& privateId, const std::string& publicId, const std::string& clientAddress);

	// Remove this client from database
	void DisconnectClient(const std::string& privateId);

	// Update this client's last connection time
	void HeartbeatClient(const std::string& privateId);

	// Update client data
	void UpdateClient(const std::string& privateId, const ClientData& data);


	// Get client data
	const ClientData& QueryClientPublic(const std::string& publicId);

	// Get client data
	const ClientData& QueryClientPrivate(const std::string& privateId);

	// List clients matching criteria
	ClientSearchResult SearchClients(const std::vector<ClientSearchCriterion>& criteria, int maxCount = 10);


private:

	// Update the database
	void BackgroundRefresh();


private:

	// Data
	std::map<std::string, std::string>              mPrivateToPublic;
	std::map<std::string, ClientData>               mData;

	// Settings
	int                                             mUpdatePeriod;
	int                                             mClientIdleTime;

	// Utils
	std::thread                                     mThread;
	std::mutex                                      mMutex;
	std::atomic<bool>                               mRunning;
	DatabaseTime                                    mStartupTime;

};
