#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

/*Structs based of slides from recitation*/
struct Attr{
    unsigned int Type;
    unsigned int Length;
    char        Payload[512];
};

struct Messages{
    unsigned int Vrsn;
    unsigned int Type;
    unsigned int Length;
    struct Attr Attr;
};

int main(int argc, char *argv[]){

    struct Messages *Mess_to;
    struct Messages *Mess_from;
    char message[512];
    int len_of_ip, readingbytes;

    int sock_fd, connection;
    char *c;

    fd_set master_fd;
    fd_set temp_fd;

    int portNum = strtol(argv[3], &c, 10);

    struct sockaddr_in serveraddress;
    sock_fd=socket(AF_INET, SOCK_STREAM,0);
    //check for error
    if(sock_fd<0){
        perror("socket error");
    }

    bzero(&serveraddress, sizeof serveraddress);
    serveraddress.sin_family=AF_INET;
    serveraddress.sin_port=htons(portNum);
    int inet = inet_pton(AF_INET, argv[2], &(serveraddress.sin_addr));
    //check for error
    if(inet <= 0){
        perror("inet error");
    }

    connection = connect(sock_fd, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
    //check for error
    if(connection < 0){
        perror("connection error");
    }

    //print message
    printf("From the client: username is %s", argv[1]);

    Mess_to = malloc(sizeof(struct Messages));
    Mess_to->Vrsn=3;
    Mess_to->Type=2;
    Mess_to->Length=24;
    Mess_to->Attr.Type=2;
    Mess_to->Attr.Length=20;
    strcpy(Mess_to->Attr.Payload, argv[1]);
    //print message
    printf("From the cleint: joining chat");

    if(write(sock_fd, Mess_to, sizeof(struct Messages))==-1){
        perror("write error");
    }

    FD_SET(0, &temp_fd);
    FD_SET(sock_fd, &temp_fd);

    while(1){
        if(select(sock_fd+1, &temp_fd, NULL, NULL, NULL)==-1){
            perror("select error");
            exit(6);
        }

        for(int i=0; i<=sock_fd; i++){
            if(FD_ISSET(i, &temp_fd)){
                if(i==0){
                    bzero(message, 512);
                    fgets(message, 512, stdin);
                    len_of_ip = strlen(message)-1;
                    
                    Mess_to = malloc(sizeof(struct Messages));
                    strcpy(Mess_to->Attr.Payload, message);
                    Mess_to->Vrsn=3;
                    Mess_to->Type=4;
                    Mess_to->Length=520;
                    Mess_to->Attr.Type=4;
                    Mess_to->Attr.Length=524;
                    if(write(sock_fd, Mess_to, sizeof(struct Messages))==-1){
                        perror("write error");
                    }
                }

                if(i==sock_fd){
                    Mess_from = malloc(sizeof(struct Messages));
                    readingbytes = read(sock_fd, Mess_from, sizeof(struct Messages));
                    //print statement
                    printf("%s", Mess_from->Attr.Payload);

                    free(Mess_from);
                    if(Mess_from->Type==5){
                        if(Mess_from->Attr.Type==1){
                            exit(7);
                        }
                    }
                }
            }
            FD_SET(0,&temp_fd);
            FD_SET(sock_fd, &temp_fd);

        }
    }
    close(sock_fd);
    return 0;
}