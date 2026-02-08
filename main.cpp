#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>      // For Multi-threading
#include <winsock2.h> 
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

unordered_map<string, string> database;

void send_response(SOCKET client_fd, string msg) {
    send(client_fd, msg.c_str(), (int)msg.length(), 0);
}

vector<string> parse_resp(char* buffer) {
    vector<string> commands;
    if (buffer[0] != '*') return commands;
    
    char* context = NULL;
    char* line = strtok_s(buffer, "\r\n", &context);
    if (!line) return commands;

    int num_args = stoi(string(line).substr(1));
    for (int i = 0; i < num_args; i++) {
        strtok_s(NULL, "\r\n", &context); 
        line = strtok_s(NULL, "\r\n", &context);
        if (line) commands.push_back(string(line));
    }
    return commands;
}

// This function runs in a separate thread for each client
void handle_client(SOCKET client_fd) {
    cout << "[NEW CONNECTION] Client ID: " << client_fd << endl;
    char buffer[1024];

    while (true) {
        memset(buffer, 0, 1024);
        int bytes = recv(client_fd, buffer, 1024, 0);
        
        if (bytes <= 0) {
            cout << "[DISCONNECTED] Client ID: " << client_fd << endl;
            break; 
        }

        cout << "[DATA RECEIVED] Raw: " << buffer << endl;

        vector<string> args = parse_resp(buffer);
        if (!args.empty()) {
            string cmd = args[0];
            if (cmd == "PING") {
                send_response(client_fd, "+PONG\r\n");
            } else if (cmd == "SET" && args.size() == 3) {
                database[args[1]] = args[2];
                send_response(client_fd, "+OK\r\n");
            } else if (cmd == "GET" && args.size() == 2) {
                if (database.count(args[1])) {
                    string val = database[args[1]];
                    send_response(client_fd, "$" + to_string(val.length()) + "\r\n" + val + "\r\n");
                } else {
                    send_response(client_fd, "$-1\r\n");
                }
            }
        }
    }
    closesocket(client_fd);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(6379), INADDR_ANY};
    
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
    
    cout << "Mini-Redis v2 (Multi-Threaded) running on port 6379..." << endl;

    while (true) {
        SOCKET client_fd = accept(server_fd, NULL, NULL);
        if (client_fd != INVALID_SOCKET) {
            // Launch a new thread for this client
            thread(handle_client, client_fd).detach();
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}