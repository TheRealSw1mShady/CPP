#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
string hexToIp(uint32_t value);

int main()
{
    int sockRequest = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    setsockopt(sockRequest, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (sockRequest < 0)
    {
        cout << "Socket error!";
        return 0;
    }

    sockaddr_in addrReq;
    addrReq.sin_family = AF_INET;
    addrReq.sin_port = htons(1488);
    addrReq.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockRequest, (struct sockaddr*)(&addrReq), sizeof(addrReq)) < 0)
    {
        cout << "Binding error!";
        return 0;
    }

    cout << "Request IP: " << hexToIp(htonl(addrReq.sin_addr.s_addr)) <<
            ", request port: " << htons(addrReq.sin_port) << endl;

}

string hexToIp(uint32_t value)
{
   return to_string(static_cast<int>(value >> 24)) + "." +
            to_string(static_cast<int>((value & 0x00FF0000) >> 16)) + "." +
            to_string(static_cast<int>((value & 0x0000FF00) >> 8)) + "." +
            to_string(static_cast<int>(value & 0x000000FF));
}
