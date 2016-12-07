#include "database.h"


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

bool Database::ConnectClient(const std::string& privateId, const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mPrivateToPublic[privateId] = publicId;

	mData[publicId] = ClientData();

	return true;
}

bool Database::DisconnectClient(const std::string& privateId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	if (mPrivateToPublic.find(privateId) != mPrivateToPublic.end())
	{
		std::string publicId = mPrivateToPublic[privateId];
		mPrivateToPublic.erase(mPrivateToPublic.find(privateId));
		mData.erase(mData.find(publicId));

		return true;
	}
	else
	{
		return false;
	}
}

bool Database::UpdateClient(const std::string& privateId, const ClientData& data)
{
	std::lock_guard<std::mutex> lock(mMutex);

	std::string publicId = mPrivateToPublic[privateId];
	mData[publicId] = data;

	return true;
}

const ClientData Database::QueryClient(const std::string& publicId)
{
	std::lock_guard<std::mutex> lock(mMutex);

	if (mData.find(publicId) != mData.end())
	{
		return mData[publicId];
	}
	else
	{
		return ClientData();
	}
}
