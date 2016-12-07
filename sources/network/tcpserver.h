#pragma once

#include <memory>
#include "network/tcpsocket.h"

class Database;


/*-----------------------------------------------------------------------------
	TcpServer class definition
-----------------------------------------------------------------------------*/

class TcpServer
{

public:

	TcpServer(std::shared_ptr<Database> pDb);

	~TcpServer();


	// Start listening on port with up to nClients clients
	void Listen(uint16_t port, uint32_t nClients);


private:

	static void workerProcess(std::shared_ptr<Database> pDatabase, TcpSocket client);


private:

	std::shared_ptr<Database>                       pDatabase;

};
