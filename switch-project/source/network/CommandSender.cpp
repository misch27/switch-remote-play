#include "CommandSender.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <string.h>
#include <assert.h>

bool ConnectTo(std::string ip, uint16_t port, int& connectionSock)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket < 0)
    {
        std::cout << "error occurred trying to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result < 0)
    {
        std::cout << "failed to connect to server: " << strerror(errno) << "(" << errno << ")" << std::endl;
        close(clientSocket);
        return false;
    }

    connectionSock = clientSocket;
    return true;
}

bool SendCommandPayload(int const connectionSock, CommandPayload const payload)
{
    static_assert(COMMAND_CODE_SIZE == 2);
    static_assert(ENCODER_CONFIG_SIZE == 24);
    static_assert(CONTROLLER_CONFIG_SIZE == 16);
    static_assert(COMMAND_PAYLOAD_SIZE == 64);

    char* dataPtr = (char*)&payload;

    // std::cout << "Sending command payload with configuration of size: " << COMMAND_PAYLOAD_SIZE << " bytes" << std::endl;

    auto result = send(connectionSock, dataPtr, COMMAND_PAYLOAD_SIZE, 0);
    if(result < 0)
    {
        std::cout << "Failed to send data code: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}