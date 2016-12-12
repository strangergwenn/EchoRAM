#pragma once

#include <string>
#include <cstdint>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


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

private:

	// This constructor is dedicated to forking the server socket when accepting a client
	TcpSocket(SOCKET socket, sockaddr_in clientInfo, SSL* pSession = nullptr);

public:

	// Copy & assigment constructors
	TcpSocket(const TcpSocket& o);
	TcpSocket& operator=(const TcpSocket& o);

	// Destructor
	~TcpSocket();


	// Setup this socket to work as a SSL server
	bool SetSSLServer(const std::string& certFile, const std::string& keyFile);

	// Setup this socket to work as a SSL client
	bool SetSSLClient(const std::string& caCertFile = "");

	// Connect to the server at url:port
	bool Connect(std::string url, uint16_t port = 80);

	// Start listening on port
	bool Listen(uint16_t port, uint32_t clients = 10);

	// Wait for connection, accept when it arrives
	const TcpSocket Accept();

	// is this socket OK ?
	bool IsValid() const;

	// Write data on the socket
	bool Write(const std::string& data);

	// Read data from the socket
	bool Read(std::string& data);

	// Get the IP address of the connected client
	std::string GetClientAddress() const;

	// Terminate the connection
	void Close();


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

	SSL_CTX*                                     mSSLContext;
	SSL*                                         mSSLSession;

	static int                                   sSocketCount;
	static addrinfo                              sConnectHints;
	static addrinfo                              sListenHints;

	static const int                             cBufferSize = 16384;
	static const int                             cPortSize = 15;

};
