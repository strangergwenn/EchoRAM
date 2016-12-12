#include "tcpserver.h"
#include "data/handler.h"
#include <thread>
#include <iostream>
#include <memory>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

TcpServer::TcpServer(std::shared_ptr<Database> pDb)
	: pDatabase(pDb)
{
}

TcpServer::~TcpServer()
{
}


/*-----------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------*/

void TcpServer::Listen(uint16_t port, uint32_t nClients, const std::string& certFile, const std::string& keyFile)
{
	TcpSocket socket;

	// Setup socket
	if (certFile.length() > 0 && keyFile.length() > 0)
	{
		socket.SetSSLServer(certFile, keyFile);
	}

	// Try listening on socket
	if (socket.Listen(port, nClients))
	{
		// Accept clients, fork the socket as a thread
		std::vector<std::thread> clients;
		while (true)
		{
			TcpSocket client = socket.Accept();
			if (client.IsValid())
			{
				clients.push_back(std::thread(ProcessClient, pDatabase, client));
			}
		}

		// Terminate connections, wait for exit
		socket.Close();
		for (auto& client : clients)
		{
			client.join();
		}
	}
}


/*-----------------------------------------------------------------------------
	Callback
-----------------------------------------------------------------------------*/

void TcpServer::ProcessClient(std::shared_ptr<Database> pDatabase, TcpSocket client)
{
	Handler handler(pDatabase, client.GetClientAddress());
	bool keepConnection = true;

	do {
		std::string request;
		std::string reply;

		keepConnection &= client.Read(request);
		keepConnection &= handler.ProcessClientRequest(request, reply);
		keepConnection &= client.Write(reply);

	} while (keepConnection);
}
