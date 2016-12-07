#pragma once

#include <string>
#include <cstdint>


/*-----------------------------------------------------------------------------
	Portability
-----------------------------------------------------------------------------*/

// Use Winsock2 on Windows
#ifdef WIN32

#  define _WINSOCK_DEPRECATED_NO_WARNINGS
#  include <winsock2.h>
#  include <ws2tcpip.h>

// Make Unix types behave as Winsock2
#else

#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <netdb.h>

#  define INVALID_SOCKET -1
#  define SOCKET_ERROR -1
#  define closesocket(s) close(s)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#endif


/*-----------------------------------------------------------------------------
	Socket class definition
-----------------------------------------------------------------------------*/

class TcpSocket
{

public:

	TcpSocket();

	TcpSocket(SOCKET socket, sockaddr_in clientInfo);

	TcpSocket(const TcpSocket& o);

	TcpSocket& operator=(const TcpSocket& o);

	~TcpSocket();


	// Connect to the server at url:port
	bool Connect(std::string url, uint16_t port = 80);

	// Start listening on port
	bool Listen(uint16_t port, uint32_t clients = 10);

	// Wait for connection, accept when it arrives
	const TcpSocket Accept();

	// Write data on the socket
	bool Write(const std::string& data);

	// Read data from the socket
	bool Read(std::string& data);

	// Terminate the connection
	void Close();

	// Get the IP address of the connected client
	std::string GetClientAddress() const;


private:

	// Setup the infrastructure
	static void Initialize();

	// Release static data
	static void Shutdown();

	// Get the last error code
	static int GetErrno();


private:

	int*                                         mRefCount;
	SOCKET                                       mSocket;
	sockaddr_in                                  mClientInfo;

	static int                                   sSocketCount;
	static addrinfo                              sConnectHints;
	static addrinfo                              sListenHints;

	static const int                             cBufferSize = 16384;
	static const int                             cPortSize = 15;

};
