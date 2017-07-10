// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <WinSock2.h>
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
		bool Init(s32 localPortNum);
		//- Init socket for sending.
		bool Init(const std::string& remoteIP, s32 remotePortNum);

		void Destroy();

		bool Send(const std::string& packet);
		bool Receive(std::string& out_packet);

	private:
		sockaddr_in localAddress;
		sockaddr_in remoteAddress;

		s32 remotePort;
		s32 localPort;
		std::string remoteIp;

		s32 inputSocket;
		s32 outputSocket;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkTCP
	{
	public:
		NetworkTCP();
	
		bool Init();
		void Destroy();
	
		//- Allow other hosts to request a TCP connection with us.
		bool OpenToConnectionRequests(s32 localPortNum);
		//- Check for pending connection requests.
		bool CheckForConnectionRequests();
	
		//- Send a connection request to IP/Port.
		bool SendConnectionRequest(const std::string& remoteIP, s32 remotePortNum);
		//- Returns true if a TCP connection is established.
		bool IsConnected() const;
	
		bool Send(const std::string& packet);
		bool Receive(std::string& out_packet);
	
	private:
		bool isConnected;

		sockaddr_in address;
		s32 remotePort;
		s32 localPort;
		std::string remoteIp;

		s32 socketId;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkClient
	{
	public:
		NetworkClient();
	
		bool Init(s32 localPortNumTCP, s32 localPortNumUDP, const std::string& remoteIP, s32 remotePortNumTCP, s32 remotePortNumUDP);
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
		s32 TCPSocket;
		s32 UDPSendSocket;
		s32 UDPReceiveSocket;
		s32 localPortTCP;
		s32 localPortUDP;
		s32 remotePortTCP;
		s32 remotePortUDP;
		std::string remoteIp;
		bool isConnected;

		char receiveBuffer[PACKET_LENGTH];
	};

	class NetworkServer
	{
	public:
		NetworkServer();
	
		bool Init(s32 localPortNumTCP, s32 localPortNumUDP);
		void Destroy();
	
		//- Allow other hosts to request a connection with us.
		bool OpenToConnectionRequests();
		//- Check for pending connection requests.
		//- Returns Unique ID.
		s32 CheckForConnectionRequests();
		//- Returns true if the ID is valid.
		bool IsConnected(s32 ID) const;

		u32 GetNumClients() const;

		void RemoveClient(s32 ID);
		
		bool SendUDP(const std::string& packet, s32 ID);
		//- Sends the message to all clients.
		bool SendUDP(const std::string& packet);
		bool SendToAllButOneUDP(const std::string& packet, s32 excludedID);
		s32 ReceiveUDP(std::string& out_packet);

		bool SendTCP(const std::string& packet, s32 ID);
		//- Sends the message to all clients
		bool SendTCP(const std::string& packet);
		bool SendToAllButOneTCP(const std::string& packet, s32 excludedID);
		s32 ReceiveTCP(std::string& out_packet);
	
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
			s32 TCPAddressSize = sizeof(sockaddr);
			s32 UDPAddressSize = sizeof(sockaddr);
			s32 TCPSocket = -1;
			s32 ID = -1;
		};

		sockaddr_in TCPAddress;
		sockaddr_in UDPAddress;
		s32 TCPSocket;
		s32 UDPSendSocket;
		s32 UDPReceiveSocket;
		s32 localPortTCP;
		s32 localPortUDP;
		s32 remotePortTCP;
		s32 remotePortUDP;
		s32 idCounter;

		char receiveBuffer[PACKET_LENGTH];

		std::vector<Client> clients;

		//- Returns the array index of the client with ID.
		//- Returns -1 if no client exists with ID.
		u32 GetClientIndex(s32 ID) const;
	};
}
