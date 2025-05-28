#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
    int clientSock, serverSock;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;

    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(10050);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        cout << "Bind error!";
        return 0;
    }
    else
        cout << "Successfully binded to IP " << addr.sin_port << " and port " << addr.sin_addr.s_addr << endl;

    listen(serverSock, 1);

    clientSock = accept(serverSock, NULL, NULL);


    if (clientSock < 0)
    {
        cout << "Bind error!";
        return 0;
    }

    while (true)
    {
        bytes_read = recv(clientSock, buf, 1024, 0);
        if (bytes_read <= 0)
            break;

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
