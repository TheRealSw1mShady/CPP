#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

using namespace std;
string hexToIp(uint32_t value);

int main()
{
    int clientSock, serverSock;
    sockaddr_in servAddr, clientAddr;
    char buf[1024];
    int bytes_read;

    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSock < 0)
    {
        cout << "Socket error!";
        return 0;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(10050);
    servAddr.sin_addr.s_addr = htonl(0x7F000001); // INADDR_ANY


    // clientAddr.sin_family = AF_INET;
    // clientAddr.sin_port = htons(8888);
    // clientAddr.sin_addr.s_addr = htonl(0);

    if (bind(serverSock, reinterpret_cast<sockaddr*>(&servAddr), sizeof(servAddr)) < 0)
    {
        cout << "Binding error!";
        return 0;
    }
    else
        cout << "Successfully binded to IP " << hexToIp(htonl(servAddr.sin_addr.s_addr)) <<
        " and port " << htons(servAddr.sin_port) << endl;

    listen(serverSock, 1);

    socklen_t size = sizeof(clientAddr);
    clientSock = accept(serverSock, (sockaddr *)&clientAddr, &size);


    if (clientSock < 0)
    {
        cout << "Acception error!";
        return 0;
    }

    while (true)
    {
        bytes_read = recv(clientSock, buf, 1024, 0);
        if (bytes_read <= 0)
            break;

        cout << "Client IP: " << hexToIp(ntohl(clientAddr.sin_addr.s_addr)) <<
                ", client port: " << ntohs(clientAddr.sin_port) << endl;
        cout << buf;

        time_t currTime = time(nullptr);
        string strTime = asctime(localtime(&currTime));

        char dateArray[strTime.length() + 1];
        dateArray[strTime.length() + 1] = '\0';

        for (int n = 0; n < strTime.length(); n++)
            dateArray[n] = strTime[n];
        send(clientSock, dateArray, sizeof(dateArray), 0);
    }

    close(clientSock);

    return 0;
}

string hexToIp(uint32_t value)
{
   return to_string(static_cast<int>(value >> 24)) + "." +
            to_string(static_cast<int>((value & 0x00FF0000) >> 16)) + "." +
            to_string(static_cast<int>((value & 0x0000FF00) >> 8)) + "." +
            to_string(static_cast<int>(value & 0x000000FF));
}
