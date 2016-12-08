#pragma once

#include "network/tcpsocket.h"

#include <random>
#include <iostream>
#include <cassert>


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

	if (reply["reply"]["status"] != std::string("OK"))
	{
		std::cout << reply["reply"]["status"] << std::endl;
		assert(false);
	}
}

int GetClientsCount(const std::string& url, int port)
{
	TcpSocket socket;
	socket.Connect(url, port);

	Json::Value stats;
	Json::Value reply;

	stats["stats"] = 1;
	SendCommandReadResult(socket, stats, reply);

	return reply["reply"]["count"].asInt();
}

