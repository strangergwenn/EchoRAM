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

void getOption(const InputParams& params, const std::string& key, const std::string& comment, std::string& value)
{
	if (params.isSet(key))
	{
		value = params.get(key);
	}
	std::cout << comment << " : " << value << std::endl;
}


int main(int argc, char** argv)
{
	// Default parameters
	int port = 8080;
	int clients = 1000;
	int dbPeriod = 5;
	int clientIdleTime = 30;
	int useSSL = 0;
	std::string publicCert = "cert.pem";
	std::string privateKey = "key.pem";

	// Start parsing
	InputParams params(argc, argv);
	std::cout << "--------------------------------------------------------------------------------" << std::endl;

	// General parameters
	getOption(params, "--port", "Listening on port", port);
	getOption(params, "--clients", "Accepting clients", clients);
	getOption(params, "--update-period", "Updating database every", dbPeriod);
	getOption(params, "--client-idle-time", "Max client idle time", clientIdleTime);

	// SSL parameters
	getOption(params, "--use-ssl", "Use SSL for encryption", useSSL);
	getOption(params, "--public-cert", "Public SSL certificate file", publicCert);
	getOption(params, "--private-key", "Private SSL key file", privateKey);

	// Done parsing
	std::cout << "--------------------------------------------------------------------------------" << std::endl;

	// Start the server
	std::shared_ptr<Database> pDatabase(new Database(dbPeriod, clientIdleTime));
	TcpServer server(pDatabase);
	if (useSSL)
	{
		server.Listen(port, clients, publicCert, privateKey);
	}
	else
	{
		server.Listen(port, clients);
	}

	// Exit
	std::cout << "Done" << std::endl;
	return EXIT_SUCCESS;
}
