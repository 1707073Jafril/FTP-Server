#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9090
#define BUFFER_SIZE 1024

void handleUpload(int clientSocket, const std::string& filename) {
    std::ofstream file("uploads/" + filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        file.write(buffer, bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Error reading from socket." << std::endl;
    }

    file.close();
    std::cout << "File uploaded successfully." << std::endl;
    send(clientSocket, "UPLOAD_SUCCESS", 15, 0);  // Acknowledge successful upload
}

void handleDownload(int clientSocket, const std::string& filename) {
    std::ifstream file("uploads/" + filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "File not found." << std::endl;
        send(clientSocket, "FILE_NOT_FOUND", 14, 0);  // Inform the client if file is not found
        return;
    }

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
    std::cout << "File downloaded successfully." << std::endl;
    send(clientSocket, "DOWNLOAD_SUCCESS", 17, 0);  // Acknowledge successful download
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Failed to listen." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "FTP server started on port " << PORT << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string command(buffer);

            if (command.substr(0, 6) == "UPLOAD") {
                std::string filename = command.substr(7);
                handleUpload(clientSocket, filename);
            } else if (command.substr(0, 8) == "DOWNLOAD") {
                std::string filename = command.substr(9);
                handleDownload(clientSocket, filename);
            } else {
                std::cerr << "Unknown command received." << std::endl;
            }
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
