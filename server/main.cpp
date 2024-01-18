#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void clientInteract(SOCKET clientSocket,vector<SOCKET>& clients) {
	cout << "client connected" << endl;
	char buffer[4096];
	while (true) {
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesrecvd <= 0) {
			cout << "client disconnected" << endl;
			break;
		}
		string message(buffer, bytesrecvd);
		cout << "message from client :" << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}
	
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);
}

int main() {

	if (!initialize()) {
		cout << "winsock initilization failed " << endl;
}
	cout << "server program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		cout << "socket creation failed" << endl;
		return 1;
	}

	const int port = 4567;
	sockaddr_in serveradd;
	serveradd.sin_family = AF_INET;
	serveradd.sin_port = htons(port);

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveradd.sin_addr) != 1) {
		cout << "setting address structure failed"<< endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveradd), sizeof(serveradd)) == SOCKET_ERROR) {
		cout << "bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}
		
	cout << "Server has started listening on port " << port << endl;

	vector<SOCKET> clients;
	while (true) {
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "invalid client socket" << endl;
		}
		clients.push_back(clientSocket);
		thread t1(clientInteract, clientSocket,ref(clients));
		t1.detach();
	}
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}