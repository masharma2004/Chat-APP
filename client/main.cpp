#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<string>
#include<thread>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
	}
bool isOnlySpaces(const string& input) {
	return input.find_first_not_of(' ') == string::npos;
}

void sndMessage(SOCKET s) {
	cout << "enter your chat name : " << endl;
	string name;
	getline(cin, name);
	string message;
	while (true) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent;
		if (!isOnlySpaces(message)) {
			 bytesent = send(s, msg.c_str(), msg.length(), 0);
		}
		if (bytesent == SOCKET_ERROR) {
			cout << " error sending message " << endl;
			break;
		}
		if (message == "quit") {
			cout << "stopping the application" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void recvMessage(SOCKET s) {
	char buffer[4096];
	int recvlen;
	string msg = "";
	while (true) {
		recvlen = recv(s, buffer, sizeof(buffer), 0);
		if (recvlen <= 0) {
			cout << "disconnected from the server" << endl;
			break;
		}
		else {
			msg = string(buffer, recvlen);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main() {
	cout << "client program started" << endl;
	if (!initialize()) {
		cout << "initialiaze winsock failed" << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket created" << endl;
		return 1;
	}

	const int port = 4567;
	sockaddr_in serveradd;
	serveradd.sin_family = AF_INET;
	serveradd.sin_port = htons(port);
	string serveraddr = "127.0.0.1";

	inet_pton(AF_INET, serveraddr.c_str(), &(serveradd.sin_addr));
	if (connect(s, reinterpret_cast<sockaddr*>(&serveradd), sizeof(serveradd)) == SOCKET_ERROR) {
		cout << "Not able to connect to server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}
	cout << "successfully connected to server" << endl;

	thread sndThread(sndMessage, s);
	thread recvThread(recvMessage, s);

	sndThread.join();
	recvThread.join();


	return 0;
}