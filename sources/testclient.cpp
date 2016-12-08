#include "json/json.h"
#include "network/tcpsocket.h"
#include <iostream>
#include <memory>
#include <random>
#include <cassert>
#include <thread>


void SendCommandReadResult(TcpSocket& socket, const Json::Value& query, Json::Value& reply)
{
	Json::Reader reader;
	Json::StreamWriterBuilder builder;

	// Write
	std::string requestData = Json::writeString(builder, query);
	socket.Write(requestData);

	// Read reply
	std::string replyData;
	reply = Json::Value();
	socket.Read(replyData);
	reader.parse(replyData, reply);
}


class Player
{
public:

	Player(std::string privateId, std::string url, int port)
	{
		// Property setup
		mPrivateId = privateId;
		std::random_device rd;
		std::mt19937 mt(rd());
		GeneratePlayerName(mt);
		GeneratePlayerLevel(mt);
		
		// Network setup
		Json::Value reply;
		mSocket.Connect(url, port);

		// Connect
		Json::Value connect;
		connect["connect"]["privateId"] = privateId;
		SendCommandReadResult(mSocket, connect, reply);
		mPublicId = reply["reply"]["publicId"].asString();
		assert(reply["reply"]["status"] == std::string("OK"));

		// Heartbeat
		Json::Value heartbeat;
		heartbeat["heartbeat"]["privateId"] = privateId;
		heartbeat["heartbeat"]["data"]["name"] = mName;
		heartbeat["heartbeat"]["data"]["level"] = std::to_string(mLevel);
		SendCommandReadResult(mSocket, heartbeat, reply);
		assert(reply["reply"]["status"] == std::string("OK"));
	}

	~Player()
	{
		// Disconnect
		Json::Value reply;
		Json::Value disconnect;
		disconnect["disconnect"]["privateId"] = mPrivateId;
		SendCommandReadResult(mSocket, disconnect, reply);
		assert(reply["reply"]["status"] == std::string("OK"));
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


private:

	std::string                                     mName;
	int                                             mLevel;
	std::string                                     mPrivateId;
	std::string                                     mPublicId;
	TcpSocket                                       mSocket;


};


int main(int argc, char** argv)
{
	Player player("123-456-789", "localhost", 1337);
	
	while (true)
	{

	}

	// Query
	/*std::cout << "Query 1" << std::endl;
	Json::Value query;
	query["query"]["targetId"] = publicId;
	SendCommandReadResult(socket, query, reply);
	std::cout << reply["reply"] << std::endl;*/

	// Search
	/*std::cout << "Search" << std::endl;
	Json::Value search;
	search["search"]["key"] = "name";
	search["search"]["value"] = "Foobar";
	SendCommandReadResult(socket, search, reply);
	std::cout << reply["reply"] << std::endl;*/
	
	// Query
	/*std::cout << "Query 2" << std::endl;
	SendCommandReadResult(socket, query, reply);
	std::cout << reply["reply"] << std::endl;*/

	return EXIT_SUCCESS;
}
