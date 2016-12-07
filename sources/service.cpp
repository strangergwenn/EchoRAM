#include "data/database.h"
#include "network/tcpserver.h"


int main(int argc, char** argv)
{
	std::shared_ptr<Database> pDatabase(new Database);

	TcpServer server(pDatabase);

	server.Listen(1337, 4);

	return EXIT_SUCCESS;
}
