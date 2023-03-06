#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    sockaddr_in listenAddr{};
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenAddr.sin_port = htons(12345);
    bind(listenSocket, reinterpret_cast<SOCKADDR*>(&listenAddr), sizeof(listenAddr));
    listen(listenSocket, SOMAXCONN);

    HANDLE completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    for (DWORD i = 0; i < systemInfo.dwNumberOfProcessors * 2; ++i) {
        HANDLE thread = CreateThread(NULL, 0, [](LPVOID lpThreadParameter) -> DWORD {
            HANDLE completionPort = reinterpret_cast<HANDLE>(lpThreadParameter);
        while (true) {
            DWORD bytesTransferred;
            SOCKET completionKey;
            LPOVERLAPPED overlapped;
            GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
            closesocket(completionKey);
            delete overlapped;
        }
            }, completionPort, 0, NULL);
        CloseHandle(thread);
    }

    while (true) {
        if (GetAsyncKeyState('Q') & 0x8000) {
            // post a completion packet with completionKey 0 to stop all worker threads
            PostQueuedCompletionStatus(completionPort, 0, 0, NULL);
            break;
        }
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);
        CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), completionPort, clientSocket, 0);
        char* buf = new char[1024];
        WSABUF wsaBuf{ static_cast<ULONG>(1024), buf };
        DWORD flags = 0;
        LPWSAOVERLAPPED overlapped = new WSAOVERLAPPED{};
        overlapped->hEvent = NULL;
        WSARecv(clientSocket, &wsaBuf, 1, NULL, &flags, overlapped, NULL);
    }
    CloseHandle(completionPort);
    std::cout << "daw";
    return 0;
}
