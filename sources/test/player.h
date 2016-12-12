#pragma once

#include "json/json.h"
#include "utils.h"
#include "network/tcpsocket.h"

#include <memory>
#include <random>
#include <cassert>
#include <openssl/sha.h>


class Player
{
public:

	Player(std::string privateId, std::string url, int port)
	{
		mPrivateId = privateId;

		std::random_device rd;
		std::mt19937 mt(rd());

		GeneratePlayerName(mt);
		GeneratePlayerLevel(mt);

		mSocket.Connect(url, port);
	}

	~Player()
	{
		Disconnect();
	}

	void Connect()
	{
		Json::Value connect;
		Json::Value reply;

		connect["connect"]["privateId"] = mPrivateId;
		connect["connect"]["publicId"] = GetPublicIdFromPrivateId(mPrivateId);
		SendCommandReadResult(mSocket, connect, reply);
		mPublicId = reply["reply"]["publicId"].asString();
	}

	void Update()
	{
		Json::Value update;
		Json::Value reply;

		update["update"]["privateId"] = mPrivateId;
		update["update"]["data"]["name"] = mName;
		update["update"]["data"]["level"] = mLevel;
		SendCommandReadResult(mSocket, update, reply);
	}

	void Heartbeat()
	{
		Json::Value heartbeat;
		Json::Value reply;

		heartbeat["heartbeat"]["privateId"] = mPrivateId;
		SendCommandReadResult(mSocket, heartbeat, reply);
	}

	std::pair<std::string, std::string> Query(std::string targetId)
	{
		Json::Value query;
		Json::Value reply;

		query["query"]["targetId"] = targetId;
		SendCommandReadResult(mSocket, query, reply);

		return std::make_pair<std::string, std::string>(
			reply["reply"]["data"]["name"].asString(),
			reply["reply"]["data"]["level"].asString());
	}

	void Disconnect()
	{
		Json::Value disconnect;
		Json::Value reply;

		disconnect["disconnect"]["privateId"] = mPrivateId;
		SendCommandReadResult(mSocket, disconnect, reply);
	}

	std::string GetPublicId() const
	{
		return mPublicId;
	}

	TcpSocket& GetSocket()
	{
		return mSocket;
	}


private:

	// Random name
	void GeneratePlayerName(std::mt19937& mt)
	{
		std::vector<std::string> begin({ "Foo", "Lo", "Ip", "Lol" });
		std::vector<std::string> end({ "bar", "rem", "sum", "wut" });

		std::uniform_int_distribution<int> idxBegin(0, (int)begin.size() - 1);
		std::uniform_int_distribution<int> idxEnd(0, (int)end.size() - 1);

		mName = begin[idxBegin(mt)] + end[idxEnd(mt)];
	}

	// Random level
	void GeneratePlayerLevel(std::mt19937& mt)
	{
		std::uniform_int_distribution<int> level(0, 50);

		mLevel = level(mt);
	}

	// Get public ID as hash of private (not really a good idea in production)
	std::string GetPublicIdFromPrivateId(const std::string privateId)
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


private:

	std::string                                     mName;
	int                                             mLevel;
	std::string                                     mPrivateId;
	std::string                                     mPublicId;
	TcpSocket                                       mSocket;


};
