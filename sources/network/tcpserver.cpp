#include "tcpserver.h"
#include "data/handler.h"
#include <thread>
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

void TcpServer::Listen(uint16_t port, uint32_t nClients)
{
	TcpSocket socket;
	socket.Listen(port, nClients);

	std::vector<std::thread> workers;
	while (true)
	{
		TcpSocket client = socket.Accept();
		workers.push_back(std::thread(workerProcess, pDatabase, client));
	}

	socket.Close();

	for (auto& worker : workers)
	{
		worker.join();
	}
}


/*-----------------------------------------------------------------------------
	Callback
-----------------------------------------------------------------------------*/

void TcpServer::workerProcess(std::shared_ptr<Database> pDatabase, TcpSocket client)
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

	client.Close();
}
