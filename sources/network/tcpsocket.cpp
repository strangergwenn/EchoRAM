#include "tcpsocket.h"
#include <iostream>


/*-----------------------------------------------------------------------------
	Constructors & destructor
-----------------------------------------------------------------------------*/

TcpSocket::TcpSocket()
{
	if (sSocketCount == 0)
	{
		Initialize();
	}
	sSocketCount++;

	mRefCount = new int(1);

	mSocket = SOCKET_ERROR;
}

TcpSocket::TcpSocket(SOCKET socket, sockaddr_in clientInfo) : TcpSocket()
{
	mSocket = socket;
	mClientInfo = clientInfo;
}

TcpSocket::TcpSocket(const TcpSocket& o)
{
	mRefCount = o.mRefCount;
	(*mRefCount)++;

	sSocketCount++;

	mSocket = o.mSocket;
	mClientInfo = o.mClientInfo;
}

TcpSocket& TcpSocket::operator=(const TcpSocket& o)
{
	(*o.mRefCount)++;
	mRefCount = o.mRefCount;

	sSocketCount++;

	mSocket = o.mSocket;
	mClientInfo = o.mClientInfo;

	return *this;
}

TcpSocket::~TcpSocket()
{
	if (--(*mRefCount) == 0)
	{
		delete mRefCount;

		Close();

		sSocketCount--;
		if (sSocketCount == 0)
		{
			Shutdown();
		}
	}
}


/*-----------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------*/

bool TcpSocket::Connect(std::string url, uint16_t port)
{
	// Create port string
	char portString[cPortSize];
	snprintf(portString, cPortSize, "%d", port);

	// Get address info
	struct addrinfo *result, *rp;
	if (getaddrinfo(url.c_str(), portString, &sConnectHints, &result) != 0)
	{
		std::cout << "Socket::Connect failed to get the address info : " << GetErrno() << std::endl;
		return false;
	}

	// Iterate results
	mSocket = SOCKET_ERROR;
	for (rp = result; rp != nullptr; rp = rp->ai_next)
	{
		mSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (mSocket == SOCKET_ERROR)
		{
			std::cout << "Socket::Connect failed to create socket : " << GetErrno() << std::endl;
			continue;
		}

		// Connect
		if (connect(mSocket, rp->ai_addr, (unsigned int)rp->ai_addrlen) != SOCKET_ERROR)
		{
			break;
		}
		else
		{
			std::cout << "Socket::Connect failed to connect : " << GetErrno() << std::endl;
		}
	}

	// Check result
	freeaddrinfo(result);
	return (mSocket != SOCKET_ERROR);
}

bool TcpSocket::Listen(uint16_t port, uint32_t clients)
{
	// Create port string
	char portString[cPortSize];
	snprintf(portString, cPortSize, "%d", port);

	// Get address info
	struct addrinfo *result, *rp;
	if (getaddrinfo(nullptr, portString, &sListenHints, &result) != 0)
	{
		std::cout << "Socket::Listen failed to get the address info : " << GetErrno() << std::endl;
		return false;
	}

	// Iterate results
	mSocket = SOCKET_ERROR;
	for (rp = result; rp != nullptr; rp = rp->ai_next)
	{
		int isReuse = 1;
		mSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (mSocket == SOCKET_ERROR)
		{
			std::cout << "Socket::Listen failed to create socket : " << GetErrno() << std::endl;
			continue;
		}
		
		// Reuse sockets
		setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&isReuse, sizeof(isReuse));

		// Bind
		if (bind(mSocket, rp->ai_addr, (unsigned int)rp->ai_addrlen) != SOCKET_ERROR)
		{
			break;
		}
		else
		{
			std::cout << "Socket::Listen failed to bind : " << GetErrno() << std::endl;
		}

	}
	freeaddrinfo(result);

	// Start listening
	if (mSocket == SOCKET_ERROR)
	{
		std::cout << "Socket::Listen failed to create socket : " << GetErrno() << std::endl;
		return false;
	}
	else if (listen(mSocket, clients) == SOCKET_ERROR)
	{
		std::cout << "Socket::Listen failed to listen : " << GetErrno() << std::endl;
		return false;
	}

	return true;
}

const TcpSocket TcpSocket::Accept()
{
	struct sockaddr_in clientInfo = { 0 };
	socklen_t clientInfoLen = sizeof(struct sockaddr_in);

	// Accept connection
	SOCKET clientSocket = accept(mSocket, (sockaddr*)&clientInfo, &clientInfoLen);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Socket::Accept failed : " << GetErrno() << std::endl;
		clientSocket = SOCKET_ERROR;
	}

	// Create a new socket instance to store the data
	return TcpSocket(clientSocket, clientInfo);
}

bool TcpSocket::Write(const std::string& data)
{
	int length = send(mSocket, (const char*)(data.data()), (int)data.size(), 0);

	return (length == data.size());
}

bool TcpSocket::Read(std::string& data)
{
	// Read into the buffer if possible
	uint8_t buffer[cBufferSize];
	int length = recv(mSocket, (char*)(buffer), cBufferSize - 1, 0);

	if (length >= 0 && length < cBufferSize - 1)
	{
		buffer[length] = '\0';
		data.assign(buffer, buffer + length);
		return true;
	}
	else
	{
		return false;
	}
}

void TcpSocket::Close()
{
	closesocket(mSocket);
}

std::string TcpSocket::GetClientAddress() const
{
	char address[16] = { 0 };

	snprintf(address, 16, "%d.%d.%d.%d",
		int (mClientInfo.sin_addr.s_addr & 0xFF),
		int((mClientInfo.sin_addr.s_addr & 0xFF00) >> 8),
		int((mClientInfo.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mClientInfo.sin_addr.s_addr & 0xFF000000) >> 24));

	return std::string(address);
}


/*-----------------------------------------------------------------------------
	Static interface
-----------------------------------------------------------------------------*/

int TcpSocket::sSocketCount = 0;
struct addrinfo TcpSocket::sConnectHints = { 0 };
struct addrinfo TcpSocket::sListenHints = { 0 };


void TcpSocket::Initialize()
{
#ifdef WIN32
	WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err < 0)
	{
		exit(EXIT_FAILURE);
	}
#endif

	sConnectHints.ai_family = AF_INET;
	sConnectHints.ai_socktype = SOCK_STREAM;
	sConnectHints.ai_flags = 0;
	sConnectHints.ai_protocol = 0;

	sListenHints.ai_family = AF_INET;
	sListenHints.ai_socktype = SOCK_STREAM;
	sListenHints.ai_flags = AI_PASSIVE;
	sListenHints.ai_protocol = 0;
}

void TcpSocket::Shutdown()
{
#ifdef WIN32
	WSACleanup();
#endif
}

int TcpSocket::GetErrno()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}
