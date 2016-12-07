#pragma once

#include <string>
#include <mutex>
#include "json/json.h"

using ClientData = std::vector<std::pair<std::string, std::string>>;


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

	// Connect this client, adding the public + private IDs in database
	bool ConnectClient(const std::string& privateId, const std::string& publicId);

	// Remove this client from database
	bool DisconnectClient(const std::string& privateId);

	// Update client data
	bool UpdateClient(const std::string& privateId, const ClientData& data);

	// Get client data
	const ClientData QueryClient(const std::string& publicId);


private:

	std::map<std::string, std::string>              mPrivateToPublic;
	std::map<std::string, ClientData>               mData;
	std::mutex                                      mMutex;

};
