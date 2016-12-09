#pragma once

#include <string>
#include <mutex>
#include "json/json.h"
#include "datatypes.h"


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

	std::map<std::string, std::string>              mPrivateToPublic;
	std::map<std::string, ClientData>               mData;
	std::mutex                                      mMutex;

};
