// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <WinSock2.h>
#include <string>
#include <string_view>
#include <vector>

namespace Jwl
{
	#define PACKET_LENGTH 512

	// Prepares Windows Sockets for use.
	bool InitWinSock();

	// Unloads the Windows Socketing system.
	void DestroyWinSock();

	class NetworkUDP
	{
	public:
		NetworkUDP();

		// Init socket for receiving.
		bool Init(int localPortNum);
		// Init socket for sending.
		bool Init(std::string_view remoteIP, int remotePortNum);

		void Destroy();

		bool Send(std::string_view packet);
		bool Receive(std::string& out_packet);

	private:
		sockaddr_in localAddress;
		sockaddr_in remoteAddress;

		int remotePort;
		int localPort;
		std::string remoteIp;

		int inputSocket;
		int outputSocket;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkTCP
	{
	public:
		NetworkTCP();

		bool Init();
		void Destroy();

		// Allow other hosts to request a TCP connection with us.
		bool OpenToConnectionRequests(int localPortNum);
		// Check for pending connection requests.
		bool CheckForConnectionRequests();

		// Send a connection request to IP/Port.
		bool SendConnectionRequest(std::string_view remoteIP, int remotePortNum);
		// Returns true if a TCP connection is established.
		bool IsConnected() const;

		bool Send(std::string_view packet);
		bool Receive(std::string& out_packet);

	private:
		bool isConnected;

		sockaddr_in address;
		int remotePort;
		int localPort;
		std::string remoteIp;

		int socketId;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkClient
	{
	public:
		NetworkClient();

		bool Init(int localPortNumTCP, int localPortNumUDP, std::string_view remoteIP, int remotePortNumTCP, int remotePortNumUDP);
		void Destroy();

		// Send a connection request to IP/Port.
		bool SendConnectionRequest();
		// Returns true if a connection is established.
		bool IsConnected() const;

		bool SendUDP(std::string_view packet);
		bool ReceiveUDP(std::string& out_packet);

		bool SendTCP(std::string_view packet);
		bool ReceiveTCP(std::string& out_packet);

	private:
		sockaddr_in TCPAddress;
		sockaddr_in UDPAddress;
		int TCPSocket;
		int UDPSendSocket;
		int UDPReceiveSocket;
		int localPortTCP;
		int localPortUDP;
		int remotePortTCP;
		int remotePortUDP;
		std::string remoteIp;
		bool isConnected;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkServer
	{
	public:
		NetworkServer();

		bool Init(int localPortNumTCP, int localPortNumUDP);
		void Destroy();

		// Allow other hosts to request a connection with us.
		bool OpenToConnectionRequests();
		// Check for pending connection requests. 
		// Returns the new client's unique ID.
		int CheckForConnectionRequests();
		// Returns true a client with ID is connected.
		bool IsConnected(int ID) const;

		unsigned GetNumClients() const;

		void RemoveClient(int ID);

		bool SendUDP(std::string_view packet, int ID);
		// Sends the message to all clients.
		bool SendUDP(std::string_view packet);
		bool SendToAllButOneUDP(std::string_view packet, int excludedID);
		int ReceiveUDP(std::string& out_packet);

		bool SendTCP(std::string_view packet, int ID);
		// Sends the message to all clients
		bool SendTCP(std::string_view packet);
		bool SendToAllButOneTCP(std::string_view packet, int excludedID);
		int ReceiveTCP(std::string& out_packet);

	private:
		struct Client
		{
			Client()
			{
				memset(&TCPAddress, 0, sizeof(sockaddr_in));
				memset(&UDPAddress, 0, sizeof(sockaddr_in));
			}

			sockaddr_in TCPAddress;
			sockaddr_in UDPAddress;
			int TCPAddressSize = sizeof(sockaddr);
			int UDPAddressSize = sizeof(sockaddr);
			int TCPSocket = -1;
			int ID = -1;
		};

		sockaddr_in TCPAddress;
		sockaddr_in UDPAddress;
		int TCPSocket;
		int UDPSendSocket;
		int UDPReceiveSocket;
		int localPortTCP;
		int localPortUDP;
		int idCounter;

		char receiveBuffer[PACKET_LENGTH];

		std::vector<Client> clients;

		// Returns the array index of the client with the ID, or -1 if no client is found.
		unsigned GetClientIndex(int ID) const;
	};
}
