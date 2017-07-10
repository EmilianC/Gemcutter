// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Network.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{
	bool InitWinSock()
	{
		WSADATA wsa;
		return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
	}

	void DestroyWinSock()
	{
		WSACleanup();
	}

#pragma region Network UDP

	NetworkUDP::NetworkUDP()
		: remotePort(-1)
		, localPort(-1)
		, inputSocket(-1)
		, outputSocket(-1)
	{
		memset(&localAddress, 0, sizeof(sockaddr_in));
		memset(&remoteAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkUDP::Init(s32 localPortNum)
	{
		localPort = localPortNum;

		if ((inputSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		localAddress.sin_family = AF_INET;
		localAddress.sin_port = htons(static_cast<u_short>(localPort));
		localAddress.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(inputSocket, (sockaddr*)&localAddress, sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		// Set as Non-Blocking.
		u_long iMode = 1;
		ioctlsocket(inputSocket, FIONBIO, &iMode);

		return true;
	}

	bool NetworkUDP::Init(const std::string& remoteIP, s32 remotePortNum)
	{
		remoteIp = remoteIP;
		remotePort = remotePortNum;

		if ((outputSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		remoteAddress.sin_family = AF_INET;
		remoteAddress.sin_port = htons(static_cast<u_short>(remotePort));
		remoteAddress.sin_addr.S_un.S_addr = inet_addr(remoteIp.c_str());
		
		// Set as Non-Blocking.
		u_long iMode = 1;
		ioctlsocket(outputSocket, FIONBIO, &iMode);

		return true;
	}

	void NetworkUDP::Destroy()
	{
		if (inputSocket != -1)
		{
			closesocket(inputSocket);
			inputSocket = -1;
		}

		if (outputSocket != -1)
		{
			closesocket(outputSocket);
			outputSocket = -1;
		}

		memset(&localAddress, 0, sizeof(sockaddr_in));
		memset(&remoteAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkUDP::Send(const std::string& packet)
	{
		return sendto(outputSocket, packet.c_str(), packet.size(), 0, (sockaddr*)&remoteAddress, sizeof(sockaddr)) != SOCKET_ERROR;
	}

	bool NetworkUDP::Receive(std::string& out_packet)
	{
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		s32 length = recvfrom(inputSocket, receiveBuffer, PACKET_LENGTH, 0, nullptr, nullptr);

		// Did we receive anything?
		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return false;
		}
		else
		{
			out_packet = receiveBuffer;
			return true;
		}
	}

#pragma endregion

#pragma region Network TCP
	
	NetworkTCP::NetworkTCP()
		: isConnected(false)
		, remotePort(-1)
		, localPort(-1)
		, socketId(-1)
	{
		memset(&address, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}
	
	bool NetworkTCP::Init()
	{
		/* Create Socket */
		if ((socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
	
		/* Setup Socket */
		address.sin_family = AF_INET;
	
		return true;
	}
	
	void NetworkTCP::Destroy()
	{
		if (isConnected)
		{
			shutdown(socketId, SD_BOTH);
		}

		if (socketId != -1)
		{
			closesocket(socketId);
			socketId = -1;
		}

		memset(&address, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}
	
	bool NetworkTCP::OpenToConnectionRequests(s32 localPortNum)
	{
		ASSERT(isConnected == false, "Network is already connected.");

		localPort = localPortNum;
	
		/* Complete socket setup */
		address.sin_port = htons(static_cast<u_short>(localPort));
		address.sin_addr.S_un.S_addr = INADDR_ANY;
	
		/* Bind our Socket */
		if (bind(socketId, (SOCKADDR*)(&address), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
	
		/* Open Socket to connections */
		listen(socketId, 1);
	
		/* Set as Non-Blocking */
		u_long iMode = 1;
		ioctlsocket(socketId, FIONBIO, &iMode);
	
		return true;
	}
	
	bool NetworkTCP::CheckForConnectionRequests()
	{
		ASSERT(isConnected == false, "Network is already connected.");
	
		SOCKET TempSock = static_cast<SOCKET>(SOCKET_ERROR);
	
		TempSock = accept(socketId, NULL, NULL);
		if (TempSock == SOCKET_ERROR)
		{
			return false;
		}
		else
		{
			// accept() gave us the new socket object to use that is connected to the other host.
			socketId = TempSock;
			isConnected = true;
			return true;
		}
	}
	
	bool NetworkTCP::SendConnectionRequest(const std::string& remoteIP, s32 remotePortNum)
	{
		ASSERT(isConnected == false, "Network is already connected.");

		remoteIp = remoteIP;
		remotePort = remotePortNum;
	
		/* Complete socket setup */
		address.sin_port = htons(static_cast<u_short>(remotePort));
		address.sin_addr.S_un.S_addr = inet_addr(remoteIp.c_str());
	
		if (connect(socketId, (SOCKADDR*)(&address), sizeof(sockaddr)) != 0)
		{
			closesocket(socketId);
			if ((socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
			{
				Destroy();
				return false;
			}
	
			return false;
		}
		else
		{
			isConnected = true;
	
			/* Set as Non-Blocking */
			u_long iMode = 1;
			ioctlsocket(socketId, FIONBIO, &iMode);
	
			return true;
		}
	}
	
	bool NetworkTCP::IsConnected() const
	{
		return isConnected;
	}
	
	bool NetworkTCP::Send(const std::string& packet)
	{
		ASSERT(isConnected == true, "Network must have a connection to call this function.");
	
		return send(socketId, packet.c_str(), packet.size(), 0) != SOCKET_ERROR;
	}
	
	bool NetworkTCP::Receive(std::string& out_packet)
	{
		ASSERT(isConnected == true, "Network must have a connection to call this function.");
		
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	
		s32 length = recv(socketId, receiveBuffer, PACKET_LENGTH, 0);
	
		// Did we receive anything?
		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return false;
		}
		else
		{
			out_packet = receiveBuffer;
			return true;
		}
	}

#pragma endregion

#pragma region Network Client

	NetworkClient::NetworkClient()
		: TCPSocket(-1)
		, UDPSendSocket(-1)
		, UDPReceiveSocket(-1)
		, localPortTCP(-1)
		, localPortUDP(-1)
		, remotePortTCP(-1)
		, remotePortUDP(-1)
		, isConnected(false)
	{
		memset(&TCPAddress, 0, sizeof(sockaddr_in));
		memset(&UDPAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkClient::Init(s32 localPortNumTCP, s32 localPortNumUDP, const std::string& remoteIP, s32 remotePortNumTCP, s32 remotePortNumUDP)
	{
		ASSERT(localPortNumTCP != localPortNumUDP, "Local TCP and UDP ports cannot be the same.");
		ASSERT(remotePortNumTCP != remotePortNumUDP, "Remote TCP and UDP ports cannot be the same.");

		localPortTCP = localPortNumTCP;
		localPortUDP = localPortNumUDP;
		remotePortTCP = remotePortNumTCP;
		remotePortUDP = remotePortNumUDP;
		remoteIp = remoteIP;

		/* Create TCP Socket */
		if ((TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		/* Create Receiving UDP Socket */
		sockaddr_in address;
		if ((UDPReceiveSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		address.sin_family = AF_INET;
		address.sin_port = htons(static_cast<u_short>(localPortUDP));
		address.sin_addr.S_un.S_addr = INADDR_ANY;
		// Bind to retrieve IP address.
		if (bind(UDPReceiveSocket, (sockaddr*)&address, sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		// Set as Non-Blocking.
		u_long iMode = 1;
		ioctlsocket(UDPReceiveSocket, FIONBIO, &iMode);

		/* Create Sending UDP Socket */
		if ((UDPSendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		UDPAddress.sin_family = AF_INET;
		UDPAddress.sin_port = htons(static_cast<u_short>(remotePortUDP));
		UDPAddress.sin_addr.S_un.S_addr = inet_addr(remoteIP.c_str());
		// Set as Non-Blocking.
		iMode = 1;
		ioctlsocket(UDPSendSocket, FIONBIO, &iMode);

		return true;
	}

	void NetworkClient::Destroy()
	{
		if (TCPSocket != -1 && isConnected)
		{
			shutdown(TCPSocket, SD_BOTH);
			closesocket(TCPSocket);
			TCPSocket = -1;
		}

		memset(&TCPAddress, 0, sizeof(sockaddr_in));
		memset(&UDPAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkClient::SendConnectionRequest()
	{
		if (isConnected)
		{
			return true;
		}

		/* Complete socket setup */
		TCPAddress.sin_family = AF_INET;
		TCPAddress.sin_port = htons(static_cast<u_short>(remotePortTCP));
		TCPAddress.sin_addr.S_un.S_addr = inet_addr(remoteIp.c_str());

		s32 result = connect(TCPSocket, (SOCKADDR*)(&TCPAddress), sizeof(sockaddr));
		if (result != 0)
		{
			//allow the socket to be used again
			//closesocket(_TCPSocket);
			//if ((_TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
			//{
			//	Destroy();
			//}
	
			return false;
		}
		else
		{
			/* Set as Non-Blocking */
			u_long iMode = 1;
			ioctlsocket(TCPSocket, FIONBIO, &iMode);

			// Send our UDP port information to the server.
			if (!SendTCP(std::to_string(localPortUDP)))
			{
				return false;
			}

			isConnected = true;
	
			return true;
		}
	}

	bool NetworkClient::IsConnected() const
	{
		return isConnected;
	}

	bool NetworkClient::SendUDP(const std::string& packet)
	{
		return sendto(UDPSendSocket, packet.c_str(), packet.size(), 0, (sockaddr*)&UDPAddress, sizeof(sockaddr)) != SOCKET_ERROR;
	}

	bool NetworkClient::ReceiveUDP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		s32 length = recvfrom(UDPReceiveSocket, receiveBuffer, PACKET_LENGTH, 0, NULL, NULL);

		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// No error, we just didn't receive a packet.
			return false;
		}
		else
		{
			out_packet = receiveBuffer;
			return true;
		}
	}

	bool NetworkClient::SendTCP(const std::string& packet)
	{
		return send(TCPSocket, packet.c_str(), packet.size(), 0) != SOCKET_ERROR;
	}

	bool NetworkClient::ReceiveTCP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		s32 length = recv(TCPSocket, receiveBuffer, PACKET_LENGTH, 0);
	
		// Did we receive anything?
		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// No error, we just didn't receive a packet.
			return false;
		}
		else
		{
			out_packet = receiveBuffer;
			return true;
		}
	}

#pragma endregion

#pragma region Network Server

	NetworkServer::NetworkServer()
		: TCPSocket(-1)
		, UDPSendSocket(-1)
		, UDPReceiveSocket(-1)
		, localPortTCP(-1)
		, localPortUDP(-1)
		, remotePortTCP(-1)
		, remotePortUDP(-1)
		, idCounter(-1)
	{
		memset(&TCPAddress, 0, sizeof(sockaddr_in));
		memset(&UDPAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkServer::Init(s32 localPortNumTCP, s32 localPortNumUDP)
	{
		ASSERT(localPortNumTCP != localPortNumUDP, "TCP and UDP ports must be different.");

		localPortTCP = localPortNumTCP;
		localPortUDP = localPortNumUDP;

		/* Create TCP Socket */
		if ((TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		/* Create Receiving UDP Socket */
		sockaddr_in address;
		if ((UDPReceiveSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		address.sin_family = AF_INET;
		address.sin_port = htons(static_cast<u_short>(localPortUDP));
		address.sin_addr.S_un.S_addr = INADDR_ANY;
		// Bind to retrieve IP address.
		if (bind(UDPReceiveSocket, (sockaddr*)&address, sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		// Set as Non-Blocking.
		u_long iMode = 1;
		ioctlsocket(UDPReceiveSocket, FIONBIO, &iMode);

		/* Create Sending UDP Socket */
		if ((UDPSendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
		// Set as Non-Blocking.
		iMode = 1;
		ioctlsocket(UDPSendSocket, FIONBIO, &iMode);

		return true;
	}

	void NetworkServer::Destroy()
	{
		if (TCPSocket != -1)
		{
			shutdown(TCPSocket, SD_BOTH);
			closesocket(TCPSocket);
			TCPSocket = -1;
		}

		memset(&TCPAddress, 0, sizeof(sockaddr_in));
		memset(&UDPAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkServer::OpenToConnectionRequests()
	{
		TCPAddress.sin_family = AF_INET;
		TCPAddress.sin_port = htons(static_cast<u_short>(localPortTCP));
		TCPAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	
		/* Bind our Socket */
		if (bind(TCPSocket, (SOCKADDR*)(&TCPAddress), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}
	
		/* Open Socket to connections */
		listen(TCPSocket, 1);
	
		/* Set as Non-Blocking */
		u_long iMode = 1;
		ioctlsocket(TCPSocket, FIONBIO, &iMode);

		return true;
	}

	s32 NetworkServer::CheckForConnectionRequests()
	{
		SOCKET tempSocket = static_cast<SOCKET>(SOCKET_ERROR);
		Client newClient;

		tempSocket = accept(TCPSocket, (sockaddr*)&newClient.TCPAddress, &newClient.TCPAddressSize);
	
		if (tempSocket == SOCKET_ERROR)
		{
			return -1;
		}
		else
		{
			/* Initialize new client */
			newClient.TCPSocket = tempSocket;
			newClient.ID = ++idCounter;

			// Wait for message with client's port.
			memset(receiveBuffer, '\0', PACKET_LENGTH);
			s32 length = SOCKET_ERROR;
			do
			{
				length = recv(newClient.TCPSocket, receiveBuffer, PACKET_LENGTH, 0);
			} while (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK);

			s32 port = std::atoi(receiveBuffer);

			// Set up address for sending data back with UDPs.
			newClient.UDPAddress.sin_family = AF_INET;
			newClient.UDPAddress.sin_port = htons(static_cast<u_short>(port));
			newClient.UDPAddress.sin_addr.S_un.S_addr = newClient.TCPAddress.sin_addr.S_un.S_addr;
			clients.push_back(newClient);

			return newClient.ID;
		}
	}

	bool NetworkServer::IsConnected(s32 ID) const
	{
		return GetClientIndex(ID) != -1;
	}

	u32 NetworkServer::GetNumClients() const
	{
		return clients.size();
	}

	void NetworkServer::RemoveClient(s32 ID)
	{
		u32 i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		shutdown(clients[i].TCPSocket, SD_BOTH);
		closesocket(clients[i].TCPSocket);

		clients.erase(clients.begin() + i);
	}

	bool NetworkServer::SendUDP(const std::string& packet, s32 ID)
	{
		u32 i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		return sendto(UDPSendSocket, packet.c_str(), packet.size(), 0, (sockaddr*)&clients[i].UDPAddress, clients[i].UDPAddressSize) != SOCKET_ERROR;
	}

	bool NetworkServer::SendUDP(const std::string& packet)
	{
		bool okay = true;
		for (u32 i = 0; i < clients.size(); i++)
		{
			bool result = sendto(UDPSendSocket, packet.c_str(), packet.size(), 0, (sockaddr*)&clients[i].UDPAddress, clients[i].UDPAddressSize) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	bool NetworkServer::SendToAllButOneUDP(const std::string& packet, s32 excludedID)
	{
		bool okay = true;
		for (u32 i = 0; i < clients.size(); i++)
		{
			if (clients[i].ID == excludedID)
				continue;

			bool result = sendto(UDPSendSocket, packet.c_str(), packet.size(), 0, (sockaddr*)&clients[i].UDPAddress, clients[i].UDPAddressSize) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	s32 NetworkServer::ReceiveUDP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		sockaddr_in addr;
		s32 addrSize = sizeof(sockaddr);

		s32 length = recvfrom(UDPReceiveSocket, receiveBuffer, PACKET_LENGTH, 0, (sockaddr*)&addr, &addrSize);

		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// No error, we just didn't receive a packet.
			return -1;
		}
		else
		{
			out_packet = receiveBuffer;

			// Identify the client.
			for (u32 i = 0; i < clients.size(); i++)
			{
				// Clients are assumed to have unique IP addresses.
				if (clients[i].UDPAddress.sin_addr.S_un.S_addr == addr.sin_addr.S_un.S_addr)
				{
					return clients[i].ID;
				}
			}
		}

		return -1;
	}

	bool NetworkServer::SendTCP(const std::string& packet, s32 ID)
	{
		u32 i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		return send(clients[i].TCPSocket, packet.c_str(), packet.size(), 0) != SOCKET_ERROR;
	}

	bool NetworkServer::SendTCP(const std::string& packet)
	{
		bool okay = true;
		for (u32 i = 0; i < clients.size(); i++)
		{
			bool result = send(clients[i].TCPSocket, packet.c_str(), packet.size(), 0) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	bool NetworkServer::SendToAllButOneTCP(const std::string& packet, s32 excludedID)
	{
		bool okay = true;
		for (u32 i = 0; i < clients.size(); i++)
		{
			if (clients[i].ID == excludedID)
				continue;

			bool result = send(clients[i].TCPSocket, packet.c_str(), packet.size(), 0) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	s32 NetworkServer::ReceiveTCP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		// Look through all clients for packets.
		for (u32 i = 0; i < clients.size(); i++)
		{
			s32 length = recv(clients[i].TCPSocket, receiveBuffer, PACKET_LENGTH, 0);
	
			// Did we receive anything?
			if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// No error, we just didn't receive a packet.
				continue;
			}
			else
			{
				out_packet = receiveBuffer;
				return clients[i].ID;
			}
		}

		return -1;
	}

	u32 NetworkServer::GetClientIndex(s32 ID) const
	{
		for (u32 i = 0; i < clients.size(); i++)
		{
			if (clients[i].ID == ID)
			{
				return i;
			}
		}
		
		return UINT_MAX;
	}

#pragma endregion

}
