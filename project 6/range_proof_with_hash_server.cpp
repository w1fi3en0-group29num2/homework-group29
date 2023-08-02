#include <iostream>
#include <iomanip>
#include <openssl/md5.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace std;
#define BUFFER_SIZE 1024

void loghex(char* message) {
    cout << hex << setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        cout << setw(2) << static_cast<uint32_t>(static_cast<unsigned char>(message[i]));
    }
    cout << endl;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "初始化Winsock库失败" << endl;
        return -1;
    }

    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddress, clientAddress;
    char buffer[BUFFER_SIZE];

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "无法创建套接字" << endl;
        WSACleanup();
        return -1;
    }

    // 设置服务器的IP地址和端口号
    serverAddress.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        cerr << "无效的IP地址" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    serverAddress.sin_port = htons(58888);

    // 绑定套接字到服务器地址
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "绑定套接字失败" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // 监听连接请求
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cerr << "监听失败" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "等待客户端连接..." << endl;

    // 接受客户端连接
    int clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "接受客户端连接失败" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "客户端已连接" << endl;

    // 与客户端进行通信
    uint8_t p[MD5_DIGEST_LENGTH] = {};
    uint8_t sigc[MD5_DIGEST_LENGTH+1] = {};
    int n = 2;
    while (n--) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(clientSocket, buffer, BUFFER_SIZE, 0);// 接收客户端消息
        cout << "接收到客户端消息: ";
        loghex(buffer);

        // 发送回复消息给客户端
        string replyMessage = "收到消息！";
        send(clientSocket, replyMessage.c_str(), (int)replyMessage.length(), 0);
        if (n == 1) {
            memcpy(p, buffer, MD5_DIGEST_LENGTH);
        }
        else {
            memcpy(sigc, buffer, MD5_DIGEST_LENGTH);
        }
    }

    //Range Proof With Hash Function
    int proofYear = 2000;
    int trustYear = 2100;
    int d1 = trustYear - proofYear;
    uint8_t MD5result[MD5_DIGEST_LENGTH];
    uint8_t tmp[MD5_DIGEST_LENGTH];
    uint8_t c_[MD5_DIGEST_LENGTH+1] = {};
    //cout << "p: ";
    //loghex((char*)p);
    MD5(p, MD5_DIGEST_LENGTH, MD5result);
    //cout << "p第一次加密: ";
    //loghex((char*)MD5result);
    for (int i = 1; i < d1; i++) {
        memcpy(tmp, MD5result, MD5_DIGEST_LENGTH);
        MD5(tmp, MD5_DIGEST_LENGTH, MD5result);
    }
    memcpy(c_, MD5result, MD5_DIGEST_LENGTH);
    if (strcmp((char*)sigc, (char*)c_)==0) {
        cout << "sigc: ";
        loghex((char*)sigc);
        cout << "c': ";
        loghex((char*)c_);
        string replyMessage = "验证成功！";
        cout << replyMessage << endl;
        send(clientSocket, replyMessage.c_str(), (int)replyMessage.length(), 0);
    }
    else {
        string replyMessage = "验证失败！";
        cout << replyMessage << endl;
        send(clientSocket, replyMessage.c_str(), (int)replyMessage.length(), 0);
    }

    // 关闭套接字
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
