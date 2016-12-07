#include "json/json.h"
#include "network/tcpsocket.h"
#include <iostream>
#include <memory>


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

int main(int argc, char** argv)
{
	// Setup
	Json::Value reply;
	TcpSocket socket;
	socket.Connect("localhost", 1337);

	// Connect
	Json::Value connect;
	std::string privateId = "123-456-789";
	connect["connect"]["privateId"] = privateId;
	SendCommandReadResult(socket, connect, reply);
	std::string publicId = reply["reply"]["publicId"].asString();

	// Heartbeat
	Json::Value heartbeat;
	heartbeat["heartbeat"]["privateId"] = privateId;
	heartbeat["heartbeat"]["data"]["name"] = "Foobar";
	heartbeat["heartbeat"]["data"]["level"] = "2";
	SendCommandReadResult(socket, heartbeat, reply);
	
	// Query
	std::cout << "Query 1" << std::endl;
	Json::Value query;
	query["query"]["targetId"] = publicId;
	SendCommandReadResult(socket, query, reply);
	std::cout << reply["reply"] << std::endl;

	// Search
	std::cout << "Search" << std::endl;
	Json::Value search;
	search["search"]["key"] = "name";
	search["search"]["value"] = "Foobar";
	SendCommandReadResult(socket, search, reply);
	std::cout << reply["reply"] << std::endl;

	// Disconnect
	Json::Value disconnect;
	disconnect["disconnect"]["privateId"] = privateId;
	SendCommandReadResult(socket, disconnect, reply);

	// Query
	std::cout << "Query 2" << std::endl;
	SendCommandReadResult(socket, query, reply);
	std::cout << reply["reply"] << std::endl;

	return EXIT_SUCCESS;
}
