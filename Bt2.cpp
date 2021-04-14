#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILENAME "db.txt"
#include <winSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32")

CRITICAL_SECTION CriticalSection;
DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);
boolean isPassed(char username_input[], char password_input[]);

SOCKET clients[64];
int numClients;

int main() {
    // Initialize the critical section one time only.
    if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
        0x00000400))
        return 0;


    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);


    numClients = 0;

    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);

        // Them client vao mang
        clients[numClients] = client;
        numClients++;

        CreateThread(0, 0, ClientThread, &client, 0, 0);

    }

    // Release resources used by the critical section object.
    DeleteCriticalSection(&CriticalSection);
    closesocket(listener);
    WSACleanup();
}


DWORD WINAPI ClientThread(LPVOID lpParam)
{
    EnterCriticalSection(&CriticalSection);
    SOCKET client = *(SOCKET*)lpParam;
    char greeting[256] = "Hay nhap username va password cua ban dang username password\n";
    char user_input[256];
    char* username_input;
    char* password_input;
    int ret;

    do {
        send(client, greeting, strlen(greeting), 0);
        ret = recv(client, user_input, sizeof(user_input), 0);

        user_input[ret] = 0;
        username_input = strtok(user_input, " ");
        password_input = strtok(NULL, " ");

        if (isPassed(username_input, password_input) == true) {

            send(client, "Dang nhap thanh cong.Hay nhap lenh ban muon su dung\n", 52, 0);
            while (true) {
                ret = recv(client, user_input, sizeof(user_input), 0);
                if (ret <= 0)
                {
                    RemoveClient(client);
                    break;
                }
                user_input[ret] = 0;
                printf("received: %s\n", user_input);
                strcpy(user_input + strlen(user_input) - 1, "");
                strcat(user_input, ">D://VissualStudioCode//out.txt");
                system(user_input);
            }
        }

        strcpy(greeting, "Ban da nhap sai, hay nhap lai\n");
    } while (isPassed(username_input, password_input) != true);


    LeaveCriticalSection(&CriticalSection);
    closesocket(client);
    return 0;
}


void RemoveClient(SOCKET client)
{
    int i = 0;
    while (clients[i] != client) i++;

    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];

    numClients--;
}

boolean isPassed(char username_input[], char password_input[]) {
    FILE* f;
    char data[60];
    char* username;
    char* password;
    f = fopen(FILENAME, "r");
    if (f == NULL) {
        printf("Cannot read");
        return 0;
    }


    while (fgets(data, 60, f) != NULL) {
        username = strtok(data, " ");
        password = strtok(NULL, " ");
        if (strcmp(username, username_input) == 0 && strcmp(password, password_input) == 0) {
            return true;
        }

    }
    fclose(f);
    return false;
}
