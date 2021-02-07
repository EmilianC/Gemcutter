// Copyright (c) 2017 Emilian Cioca
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Network.h"
#include "gemcutter/Application/Logging.h"

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

	bool NetworkUDP::Init(int localPortNum)
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
		if (bind(inputSocket, reinterpret_cast<sockaddr*>(&localAddress), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			Destroy();
			return false;
		}

		// Set as Non-Blocking.
		u_long iMode = 1;
		ioctlsocket(inputSocket, FIONBIO, &iMode);

		return true;
	}

	bool NetworkUDP::Init(std::string_view remoteIP, int remotePortNum)
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

	bool NetworkUDP::Send(std::string_view packet)
	{
		return sendto(outputSocket, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&remoteAddress), sizeof(sockaddr)) != SOCKET_ERROR;
	}

	bool NetworkUDP::Receive(std::string& out_packet)
	{
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		int length = recvfrom(inputSocket, receiveBuffer, PACKET_LENGTH, 0, nullptr, nullptr);

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

	bool NetworkTCP::OpenToConnectionRequests(int localPortNum)
	{
		ASSERT(!isConnected, "Network is already connected.");

		localPort = localPortNum;

		/* Complete socket setup */
		address.sin_port = htons(static_cast<u_short>(localPort));
		address.sin_addr.S_un.S_addr = INADDR_ANY;

		/* Bind our Socket */
		if (bind(socketId, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr)) == SOCKET_ERROR)
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
		ASSERT(!isConnected, "Network is already connected.");

		auto TempSock = static_cast<SOCKET>(SOCKET_ERROR);

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

	bool NetworkTCP::SendConnectionRequest(std::string_view remoteIP, int remotePortNum)
	{
		ASSERT(!isConnected, "Network is already connected.");

		remoteIp = remoteIP;
		remotePort = remotePortNum;

		/* Complete socket setup */
		address.sin_port = htons(static_cast<u_short>(remotePort));
		address.sin_addr.S_un.S_addr = inet_addr(remoteIp.c_str());

		if (connect(socketId, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr)) != 0)
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

	bool NetworkTCP::Send(std::string_view packet)
	{
		ASSERT(isConnected, "Network must have a connection to call this function.");

		return send(socketId, packet.data(), packet.size(), 0) != SOCKET_ERROR;
	}

	bool NetworkTCP::Receive(std::string& out_packet)
	{
		ASSERT(isConnected, "Network must have a connection to call this function.");

		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		int length = recv(socketId, receiveBuffer, PACKET_LENGTH, 0);

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

	bool NetworkClient::Init(int localPortNumTCP, int localPortNumUDP, std::string_view remoteIP, int remotePortNumTCP, int remotePortNumUDP)
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
		if (bind(UDPReceiveSocket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr)) == SOCKET_ERROR)
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
		UDPAddress.sin_addr.S_un.S_addr = inet_addr(remoteIP.data());
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

		int result = connect(TCPSocket, reinterpret_cast<sockaddr*>(&TCPAddress), sizeof(sockaddr));
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

	bool NetworkClient::SendUDP(std::string_view packet)
	{
		return sendto(UDPSendSocket, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&UDPAddress), sizeof(sockaddr)) != SOCKET_ERROR;
	}

	bool NetworkClient::ReceiveUDP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		int length = recvfrom(UDPReceiveSocket, receiveBuffer, PACKET_LENGTH, 0, NULL, NULL);

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

	bool NetworkClient::SendTCP(std::string_view packet)
	{
		return send(TCPSocket, packet.data(), packet.size(), 0) != SOCKET_ERROR;
	}

	bool NetworkClient::ReceiveTCP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		int length = recv(TCPSocket, receiveBuffer, PACKET_LENGTH, 0);

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

	NetworkServer::NetworkServer()
		: TCPSocket(-1)
		, UDPSendSocket(-1)
		, UDPReceiveSocket(-1)
		, localPortTCP(-1)
		, localPortUDP(-1)
		, idCounter(-1)
	{
		memset(&TCPAddress, 0, sizeof(sockaddr_in));
		memset(&UDPAddress, 0, sizeof(sockaddr_in));
		memset(receiveBuffer, '\0', PACKET_LENGTH);
	}

	bool NetworkServer::Init(int localPortNumTCP, int localPortNumUDP)
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
		if (bind(UDPReceiveSocket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr)) == SOCKET_ERROR)
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
		if (bind(TCPSocket, reinterpret_cast<sockaddr*>(&TCPAddress), sizeof(sockaddr)) == SOCKET_ERROR)
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

	int NetworkServer::CheckForConnectionRequests()
	{
		Client newClient;
		SOCKET tempSocket = accept(TCPSocket, reinterpret_cast<sockaddr*>(&newClient.TCPAddress), &newClient.TCPAddressSize);

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
			while (recv(newClient.TCPSocket, receiveBuffer, PACKET_LENGTH, 0) == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
			{}

			int port = std::atoi(receiveBuffer);

			// Set up address for sending data back with UDPs.
			newClient.UDPAddress.sin_family = AF_INET;
			newClient.UDPAddress.sin_port = htons(static_cast<u_short>(port));
			newClient.UDPAddress.sin_addr.S_un.S_addr = newClient.TCPAddress.sin_addr.S_un.S_addr;
			clients.push_back(newClient);

			return newClient.ID;
		}
	}

	bool NetworkServer::IsConnected(int ID) const
	{
		return GetClientIndex(ID) != -1;
	}

	unsigned NetworkServer::GetNumClients() const
	{
		return clients.size();
	}

	void NetworkServer::RemoveClient(int ID)
	{
		unsigned i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		shutdown(clients[i].TCPSocket, SD_BOTH);
		closesocket(clients[i].TCPSocket);

		clients.erase(clients.begin() + i);
	}

	bool NetworkServer::SendUDP(std::string_view packet, int ID)
	{
		unsigned i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		return sendto(UDPSendSocket, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&clients[i].UDPAddress), clients[i].UDPAddressSize) != SOCKET_ERROR;
	}

	bool NetworkServer::SendUDP(std::string_view packet)
	{
		bool okay = true;
		for (auto& client : clients)
		{
			bool result = sendto(UDPSendSocket, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&client.UDPAddress), client.UDPAddressSize) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	bool NetworkServer::SendToAllButOneUDP(std::string_view packet, int excludedID)
	{
		bool okay = true;
		for (auto& client : clients)
		{
			if (client.ID == excludedID)
				continue;

			bool result = sendto(UDPSendSocket, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&client.UDPAddress), client.UDPAddressSize) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	int NetworkServer::ReceiveUDP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		sockaddr_in addr;
		int addrSize = sizeof(sockaddr);

		int length = recvfrom(UDPReceiveSocket, receiveBuffer, PACKET_LENGTH, 0, reinterpret_cast<sockaddr*>(&addr), &addrSize);

		if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// No error, we just didn't receive a packet.
			return -1;
		}
		else
		{
			out_packet = receiveBuffer;

			// Identify the client.
			for (auto& client : clients)
			{
				// Clients are assumed to have unique IP addresses.
				if (client.UDPAddress.sin_addr.S_un.S_addr == addr.sin_addr.S_un.S_addr)
				{
					return client.ID;
				}
			}
		}

		return -1;
	}

	bool NetworkServer::SendTCP(std::string_view packet, int ID)
	{
		unsigned i = GetClientIndex(ID);
		ASSERT(i != -1, "Client with ID %d could not be found.", ID);

		return send(clients[i].TCPSocket, packet.data(), packet.size(), 0) != SOCKET_ERROR;
	}

	bool NetworkServer::SendTCP(std::string_view packet)
	{
		bool okay = true;
		for (auto& client : clients)
		{
			bool result = send(client.TCPSocket, packet.data(), packet.size(), 0) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	bool NetworkServer::SendToAllButOneTCP(std::string_view packet, int excludedID)
	{
		bool okay = true;
		for (auto& client : clients)
		{
			if (client.ID == excludedID)
				continue;

			bool result = send(client.TCPSocket, packet.data(), packet.size(), 0) != SOCKET_ERROR;
			okay = okay && result;
		}

		return okay;
	}

	int NetworkServer::ReceiveTCP(std::string& out_packet)
	{
		// Empty buffers.
		out_packet.clear();
		memset(receiveBuffer, '\0', PACKET_LENGTH);

		// Look through all clients for packets.
		for (auto& client : clients)
		{
			int length = recv(client.TCPSocket, receiveBuffer, PACKET_LENGTH, 0);

			// Did we receive anything?
			if (length == SOCKET_ERROR || WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// No error, we just didn't receive a packet.
				continue;
			}
			else
			{
				out_packet = receiveBuffer;
				return client.ID;
			}
		}

		return -1;
	}

	unsigned NetworkServer::GetClientIndex(int ID) const
	{
		for (unsigned i = 0; i < clients.size(); ++i)
		{
			if (clients[i].ID == ID)
			{
				return i;
			}
		}

		return UINT_MAX;
	}
}
