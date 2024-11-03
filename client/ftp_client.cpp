#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9090
#define BUFFER_SIZE 1024

void uploadFile(int clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "File not found." << std::endl;
        return;
    }

    send(clientSocket, ("UPLOAD " + filename).c_str(), filename.length() + 7, 0);

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();

    char response[BUFFER_SIZE];
    recv(clientSocket, response, sizeof(response), 0);
    std::cout << "Server response: " << response << std::endl;  // Print server response
}

void downloadFile(int clientSocket, const std::string& filename) {
    send(clientSocket, ("DOWNLOAD " + filename).c_str(), filename.length() + 9, 0);

    std::ofstream file("downloads/" + filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        file.write(buffer, bytesReceived);
    }

    file.close();

    char response[BUFFER_SIZE];
    recv(clientSocket, response, sizeof(response), 0);
    std::cout << "Server response: " << response << std::endl;  // Print server response
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        return 1;
    }

    std::string command, filename;
    std::cout << "Enter command (UPLOAD/DOWNLOAD filename): ";
    std::cin >> command >> filename;

    if (command == "UPLOAD") {
        uploadFile(clientSocket, filename);
    } else if (command == "DOWNLOAD") {
        downloadFile(clientSocket, filename);
    } else {
        std::cerr << "Unknown command." << std::endl;
    }

    close(clientSocket);
    return 0;
}
