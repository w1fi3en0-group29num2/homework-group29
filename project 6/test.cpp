#include <iostream>
#include <string>
#include <iomanip>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <random>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace std;
#define SEED_LENGTH 16
#define BUFFER_SIZE 1024

void loghex(char* message) {
    cout << hex << setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        cout << setw(2) << static_cast<uint32_t>(static_cast<unsigned char>(message[i]));
    }
    cout << endl;
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "初始化Winsock库失败" << endl;
        return -1;
    }

    SOCKET clientSocket;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];

    // 创建套接字
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "无法创建套接字" << endl;
        WSACleanup();
        return -1;
    }

    // 设置服务器地址
    serverAddress.sin_family = AF_INET;
    //serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));
    serverAddress.sin_port = htons(58888);

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cerr << "连接到服务器失败" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    cout << "已连接到服务器" << endl;

    // 与服务器进行通信
    int n = 1;
    while (n--)
    {
        // 发送消息给服务器
        string message;
        cout << "证明Alice的年龄 > 21: "<<endl;

        random_device rd;//用于生成随机数种子
        mt19937 r_eng(rd());//随机数生成器  
        uniform_int_distribution<uint32_t> dis(0, 255);//随机数分布器 闭区间
        uint8_t seed[SEED_LENGTH] = {};
        for (int i = 0; i < SEED_LENGTH; ++i)
        {
            seed[i]=(uint8_t)dis(r_eng);
        }
        //s = H0(seed)
        uint8_t s[SHA256_DIGEST_LENGTH];
        SHA256(seed, SEED_LENGTH, s);

        int burnYear = 1978;
        int proofYear = 2000;
        int trustYear = 2100;
        int k = trustYear - burnYear;
        int d0 = proofYear - burnYear;
        //c = H1(s)^k
        uint8_t MD5result[MD5_DIGEST_LENGTH];
        uint8_t tmp[MD5_DIGEST_LENGTH];
        uint8_t p[MD5_DIGEST_LENGTH] = {};
        uint8_t sigc[MD5_DIGEST_LENGTH];
        MD5(s, SHA256_DIGEST_LENGTH, MD5result);
        for (int i = 1; i < k; i++) {
            if (i == d0) {
                memcpy(p, MD5result, MD5_DIGEST_LENGTH);
            }
            memcpy(tmp, MD5result, MD5_DIGEST_LENGTH);
            MD5(tmp, MD5_DIGEST_LENGTH, MD5result);
        }
        memcpy(sigc, MD5result, MD5_DIGEST_LENGTH);
        loghex((char*)MD5result);
        if (k > d0) {
            send(clientSocket, (char*)p, MD5_DIGEST_LENGTH, 0);
        }
        send(clientSocket, (char*)sigc, MD5_DIGEST_LENGTH, 0);

        // 接收服务器的回复消息
        memset(buffer, 0, BUFFER_SIZE);
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        cout << "接收到服务器回复消息: " << buffer << endl;
        memset(buffer, 0, BUFFER_SIZE);
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        cout << "接收到服务器回复消息: " << buffer << endl;
        memset(buffer, 0, BUFFER_SIZE);
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        cout << "接收到服务器回复消息: " << buffer << endl;
    }

    // 关闭套接字
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}