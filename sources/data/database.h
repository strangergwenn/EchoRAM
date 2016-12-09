#pragma once

#include <string>
#include <map>
#include <mutex>
#include <chrono>
#include "clientattribute.h"


/*-----------------------------------------------------------------------------
	Database types
-----------------------------------------------------------------------------*/

// Search criteria
enum class SearchCriteriaType { T_EQUAL = 0, T_LESSER, T_GREATER, T_LESSER_EQ, T_GREATER_EQ };

// Time
using DatabaseTime = std::chrono::time_point<std::chrono::steady_clock>;


// Map of client attributes
class ClientData
{
public:

	ClientData()
	{
		lastUpdateTime = std::chrono::high_resolution_clock::now();
	}

	ClientData(const std::string& id, const std::string& addr)
		: privateId(id)
		, clientAddress(addr)
	{
		lastUpdateTime = std::chrono::high_resolution_clock::now();
	}

public:

	std::string                              privateId;
	std::string                              clientAddress;
	std::map<std::string, ClientAttribute>   attributes;
	DatabaseTime                             lastUpdateTime;

};


// Search result for a value
using ClientSearchResult = std::map<std::string, ClientData>;


/*-----------------------------------------------------------------------------
	Database class definition
-----------------------------------------------------------------------------*/

class Database
{
public:

	Database();

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

	// Update client data
	void UpdateClient(const std::string& privateId, const ClientData& data);


	// Get client data
	const ClientData& QueryClient(const std::string& publicId);

	// List clients matching criteria
	ClientSearchResult SearchClients(const std::string& key, const ClientAttribute& value, SearchCriteriaType criteria, int maxCount = 10);


private:

	// Data
	std::map<std::string, std::string>              mPrivateToPublic;
	std::map<std::string, ClientData>               mData;

	// Utils
	std::mutex                                      mMutex;
	DatabaseTime                                    mStartupTime;

};
