#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "iostream"

using namespace std;

string hexToIp(uint32_t value);

char message[] = "What time is it?\n";
char buffer[sizeof(message)];

int main()
{
    int sock;
    sockaddr_in serverAddr, myAddr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cout << "Socket error";
        return 0;
    }

    /* Отключение TIME_WAIT, чтобы сервер не держал сокет
     * клиента занятым некоторое время после отключения клиента */
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(10050);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    // 127.0.0.1

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(9999);
    myAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(sock, (sockaddr *)&myAddr, sizeof(myAddr));

    cout << "Client IP: " << hexToIp(htonl(myAddr.sin_addr.s_addr)) << ", client port: " << htons(myAddr.sin_port) << endl;


    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Connection error";
        return 0;
    }

    send(sock, message, sizeof(message), 0);

    int bytes_read;
    while (true)
    {
        bytes_read = recv(sock, buffer, 1024, 0);
        cout << buffer;
        break;
    }

    close(sock);

    return 0;
}

string hexToIp(uint32_t value)
{
   return to_string(static_cast<int>(value >> 24)) + "." +
            to_string(static_cast<int>((value & 0x00FF0000) >> 16)) + "." +
            to_string(static_cast<int>((value & 0x0000FF00) >> 8)) + "." +
            to_string(static_cast<int>(value & 0x000000FF));
}
