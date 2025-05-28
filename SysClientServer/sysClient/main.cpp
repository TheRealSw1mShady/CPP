#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "iostream"

using namespace std;

char message[] = "What time is it?\n";
char buffer[sizeof(message)];

int main()
{
    int sock;
    sockaddr_in serverAddr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cout << "Socket error";
        return 0;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(10050);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

    sockaddr_in myAddr;
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(2498);
    myAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(sock, (sockaddr *)&myAddr, sizeof(myAddr));

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
    }

    close(sock);

    return 0;
}
