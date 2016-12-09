#include "inputparams.h"
#include "data/database.h"
#include "network/tcpserver.h"

#include <string>
#include <iostream>


void getOption(const InputParams& params, const std::string& key, const std::string& comment, int& value)
{
	if (params.isSet(key))
	{
		value = stoi(params.get(key));
	}
	std::cout << comment << " : " << value << std::endl;
}


int main(int argc, char** argv)
{
	// Default values
	int port = 8080;
	int clients = 1000;
	int dbPeriod = 5;
	int clientIdleTime = 30;

	// Parse parameters
	InputParams params(argc, argv);
	getOption(params, "--port", "Listening on port", port);
	getOption(params, "--clients", "Accepting clients", clients);
	getOption(params, "--update-period", "Updating database every", dbPeriod);
	getOption(params, "--client-idle-time", "Max client idle time", clientIdleTime);

	// Server operation
	std::shared_ptr<Database> pDatabase(new Database(dbPeriod, clientIdleTime));
	TcpServer server(pDatabase);
	server.Listen(port, clients);

	// Exit
	std::cout << "Done" << std::endl;
	return EXIT_SUCCESS;
}
