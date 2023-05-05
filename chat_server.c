#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
int main() 
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    fd_set fdread;
    int clients[64];
    int num_clients = 0;
    char buf[256];
    char* name[64];
    while (1)
    {   
        // printf("wating");
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        
        for (int i = 0; i < num_clients; i++)
            FD_SET(clients[i], &fdread);

        int ret = select(FD_SETSIZE, &fdread, NULL, NULL, NULL);

        if (FD_ISSET(listener, &fdread))
        {
            int client = accept(listener, NULL, NULL);
            // TODO: Kiem tra gioi han
            clients[num_clients++] = client;
            printf("New client connected %d\n", client);
            char request[64] = "Enter client_id: client_name :";
            send(client,request,strlen(request),0);
        }
        for (int i = 0; i < num_clients; i++){

            if (FD_ISSET(clients[i], &fdread))
            {   
                int ret = recv(clients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    continue;
                }
                buf[ret] = 0;
                 if (strncmp(buf,"client_id: ",11)==0 && strlen(buf)>12){
                char* client_name = malloc(strlen(buf)-12);
                memcpy(client_name,buf+11,strlen(buf)-12);
                // printf("%ld, %ld\n",strlen(buf),sizeof(client_name));
                name[clients[i]] = malloc (strlen(client_name));
                memcpy(name[clients[i]],client_name,strlen(client_name));
                printf("client is accepteded, name is %s len is %s\n",client_name,name[clients[i]]);
     
                continue;
        }
    
                
                char* sendmsg = NULL;
                sendmsg = malloc(256);
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                if (tm.tm_hour< 12)
                sprintf(sendmsg,"%d/%d/%d %d:%d:%dAM %s: %s",
                tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour
                ,tm.tm_min,tm.tm_sec,name[clients[i]],buf);

                else  
                sprintf(sendmsg,"%d/%d/%d %d:%d:%dPM %s: %s",
                tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour
                ,tm.tm_min,tm.tm_sec,name[clients[i]],buf);
                printf("Received from %s: %s\n", name[clients[i]], buf);
                for (int j=0;j<num_clients;j++){
                    if (i == j) continue;
                    send(clients[j],sendmsg,strlen(sendmsg),0);
                }
            }
        }
    }
    
    close(listener);    

    return 0;
    }
