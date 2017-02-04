// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <vector>

namespace Jwl
{
	#define PACKET_LENGTH 512

	//- Prepares Windows Sockets for use.
	bool InitWinSock();

	//- Unloads the Windows Socketing system.
	void DestroyWinSock();

	class NetworkUDP
	{
	public:
		NetworkUDP();

		//- Init socket for receiving.
		bool Init(int localPortNum);
		//- Init socket for sending.
		bool Init(const std::string& remoteIP, int remotePortNum);

		void Destroy();

		bool Send(const std::string& packet);
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
	
		//- Allow other hosts to request a TCP connection with us.
		bool OpenToConnectionRequests(int localPortNum);
		//- Check for pending connection requests.
		bool CheckForConnectionRequests();
	
		//- Send a connection request to IP/Port.
		bool SendConnectionRequest(const std::string& remoteIP, int remotePortNum);
		//- Returns true if a TCP connection is established.
		bool IsConnected() const;
	
		bool Send(const std::string& packet);
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
	
		bool Init(int localPortNumTCP, int localPortNumUDP, const std::string& remoteIP, int remotePortNumTCP, int remotePortNumUDP);
		void Destroy();
	
		//- Send a connection request to IP/Port.
		bool SendConnectionRequest();
		//- Returns true if a connection is established.
		bool IsConnected() const;

		bool SendUDP(const std::string& packet);
		bool ReceiveUDP(std::string& out_packet);

		bool SendTCP(const std::string& packet);
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
	
		//- Allow other hosts to request a connection with us.
		bool OpenToConnectionRequests();
		//- Check for pending connection requests.
		//- Returns Unique ID.
		int CheckForConnectionRequests();
		//- Returns true if the ID is valid.
		bool IsConnected(int ID) const;

		unsigned GetNumClients() const;

		void RemoveClient(int ID);
		
		bool SendUDP(const std::string& packet, int ID);
		//- Sends the message to all clients.
		bool SendUDP(const std::string& packet);
		bool SendToAllButOneUDP(const std::string& packet, int excludedID);
		int ReceiveUDP(std::string& out_packet);

		bool SendTCP(const std::string& packet, int ID);
		//- Sends the message to all clients
		bool SendTCP(const std::string& packet);
		bool SendToAllButOneTCP(const std::string& packet, int excludedID);
		int ReceiveTCP(std::string& out_packet);
	
	private:
		struct _Client_
		{
			_Client_()
				: TCPAddressSize(sizeof(sockaddr))
				, UDPAddressSize(sizeof(sockaddr))
				, TCPSocket(-1)
				, ID(-1)
			{
				memset((char*)&TCPAddress, 0, sizeof(sockaddr_in));
				memset((char*)&UDPAddress, 0, sizeof(sockaddr_in));
			}

			sockaddr_in TCPAddress;
			sockaddr_in UDPAddress;
			int TCPAddressSize;
			int UDPAddressSize;
			int TCPSocket;
			int ID;
		};

		sockaddr_in TCPAddress;
		sockaddr_in UDPAddress;
		int TCPSocket;
		int UDPSendSocket;
		int UDPReceiveSocket;
		int localPortTCP;
		int localPortUDP;
		int remotePortTCP;
		int remotePortUDP;
		int idCounter;

		char receiveBuffer[PACKET_LENGTH];

		std::vector<_Client_> clients;

		//- Returns the array index of the client with ID.
		//- Returns -1 if no client exists with ID.
		unsigned GetClientIndex(int ID) const;
	};
}
