#include "inputparams.h"
#include "data/database.h"
#include "network/tcpserver.h"

#include <string>
#include <iostream>


int main(int argc, char** argv)
{
	InputParams params(argc, argv);

	// Get port
	int port = 8080;
	if (params.isSet("--port"))
	{
		port = stoi(params.get("--port"));
	}
	std::cout << "Listening on port " << port << std::endl;

	// Get max clients
	int clients = 1000;
    if (params.isSet("--clients"))
    {
            clients = stoi(params.get("--clients"));
    }
	std::cout << "Accepting up to  " << clients << " clients" << std::endl;

	// Server operation
	std::shared_ptr<Database> pDatabase(new Database);
	TcpServer server(pDatabase);
	server.Listen(port, clients);

	// Exit
	std::cout << "Done" << std::endl;
	return EXIT_SUCCESS;
}
