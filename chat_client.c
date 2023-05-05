#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000); 

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        return 1;
    }
    
    char init[68];
    char *client_id = malloc(64);
    char buf[256];

    int ret = recv(client, buf, sizeof(buf), 0);
    buf[ret] = 0;
    printf("Received: %s\n", buf);
    while(1){
        fgets(init, sizeof(init), stdin);
        memcpy(client_id,init+11,strlen(init)-12);
        if (strncmp(init,"client_id: ",11)==0 && strlen(init)>11)
        {       
                break;
        }
        else {printf("Wrong format enter format : client_id: client_name :\n");}
        }
    send(client, init, sizeof(init), 0);
    fd_set fdread, fdtest;
    
    FD_ZERO(&fdread);
    FD_SET(STDIN_FILENO, &fdread);
    FD_SET(client, &fdread);
   
    while (1)
    {
        fdtest = fdread;
        int ret = select(client + 1, &fdtest, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &fdtest))
        {   
            fgets(buf, sizeof(buf), stdin);
            send(client, buf, strlen(buf), 0);
        }

        if (FD_ISSET(client, &fdtest))
        {
            ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
                break;
            buf[ret] = 0;
            printf("%s\n", buf);
        }
    }

    // Ket thuc, dong socket
    close(client);

    return 0;
}