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
		std::cout << "SendCommandReadResult failed : reply was " << reply["reply"]["status"] << std::endl;
		assert(false);
	}
}

