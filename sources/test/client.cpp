#include "player.h"
#include "utils.h"
#include "json/json.h"
#include "network/tcpsocket.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>


static std::mutex sPrintMutex;


void SimulateRandomClient(std::string url, int port, int identifier)
{
	// Random
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> randomTime(0, 5);
	std::uniform_int_distribution<int> randomHeartbeatCount(1, 3);

	// Simulate connection
	Player player(std::to_string(identifier), url, port);
	std::this_thread::sleep_for(std::chrono::seconds(randomTime(mt)));
	player.Connect();
	std::this_thread::sleep_for(std::chrono::seconds(randomTime(mt)));

	// Simulate lifetime
	for (int i = 0; i < randomHeartbeatCount(mt); i++)
	{
		player.Heartbeat();
		std::this_thread::sleep_for(std::chrono::seconds(randomTime(mt)));
	}
}


void SimulateQueryClient(std::string url, int port, int identifier)
{
	// Random
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> randomTime(0, 2);

	// Simulate connection
	Player player(std::to_string(identifier), url, port);
	player.Connect();
	{
		std::lock_guard<std::mutex> lock(sPrintMutex);
		std::cout << "Query client connected" << std::endl;
	}

	// Simulate heartbeat
	std::this_thread::sleep_for(std::chrono::seconds(randomTime(mt)));
	player.Heartbeat();
	{
		std::lock_guard<std::mutex> lock(sPrintMutex);
		std::cout << "Query client ready" << std::endl;
	}

	// Wait
	std::this_thread::sleep_for(std::chrono::seconds(randomTime(mt)));
	{
		std::lock_guard<std::mutex> lock(sPrintMutex);
		std::cout << "Query client looking for player" << std::endl;
	}

	// Search
	Json::Value search;
	Json::Value reply;
	search["search"]["key"] = "level";
	search["search"]["criteria"] = ">";
	search["search"]["value"] = 30;
	SendCommandReadResult(player.GetSocket(), search, reply);

	// Print results
	{
		std::lock_guard<std::mutex> lock(sPrintMutex);
		std::cout << reply["reply"] << std::endl;
	}

	// Search
	Json::Value stats;
	reply = Json::Value();
	stats["stats"]["key"] = 1;
	SendCommandReadResult(player.GetSocket(), stats, reply);

	// Print results
	{
		std::lock_guard<std::mutex> lock(sPrintMutex);
		std::cout << reply["reply"] << std::endl;
	}

}


int main(int argc, char** argv)
{
	// Setup
	std::string serverAddress = "localhost";
	int serverPort = 8080;
	int clientCount = 100;

	// Create simulated clients
	std::vector<std::thread> clients;
	for (int i = 1; i <= clientCount; i++)
	{
		clients.push_back(std::thread(SimulateRandomClient, serverAddress, serverPort, i));
	}
	clients.push_back(std::thread(SimulateQueryClient, serverAddress, serverPort, clientCount + 1));

	// Wait for clients to finish
	for (auto& client : clients)
	{
		client.join();
	}

	// Client count
	int finalClientCount = GetClientsCount(serverAddress, serverPort);
	assert(finalClientCount == 0);

	return EXIT_SUCCESS;
}
