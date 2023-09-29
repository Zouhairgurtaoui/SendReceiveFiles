#include "transfer.h"

void receive_file(SOCKET ,FILE* );
ssize_t totale = 0;

void* get_in_addr(struct sockaddr *sa){

    if(sa->sa_family == AF_INET)return &(((struct sockaddr_in*)sa)->sin_addr);
    else return  &(((struct sockaddr_in6*)sa)->sin6_addr);

}

int main(int argc,char* argv[]){

    SOCKET servfd,connfd;
    WSADATA wsadata;
    WORD wVersionRequested = MAKEWORD(2,2);
    int wsaerr;

    wsaerr = WSAStartup(wVersionRequested,&wsadata);
    if(wsaerr != 0){
        fprintf(stderr,"Error loading dll %s\n",WSAGetLastError());
        WSACleanup();
        return -1;
    }

    struct addrinfo hints,*servinfo,*p;
    struct sockaddr_storage clientaddr;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    int rv = getaddrinfo("192.168.56.1",SERVERPORT,&hints,&servinfo);
    if(rv != 0){
        fprintf(stderr,"getaddrinfo : %s\n",gai_strerror(rv));
        return -1;
        WSACleanup();
    }

   

    printf("waiting for connection ...");
    for(p = servinfo;p != NULL;p=p->ai_next){
         servfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    if(servfd == -1){
        perror("Creating Socket");
        continue;
    }

    if(bind(servfd,servinfo->ai_addr,sizeof(struct sockaddr)) == SOCKET_ERROR){
        perror("Binding");
        printf("%d",WSAGetLastError());
        continue;
    }

    if(listen(servfd,BACKLOG) == SOCKET_ERROR){
        perror("Listining");
       continue;
    }
      
        break;
    }

    freeaddrinfo(servinfo);
    if(!p){
        perror("server : didn't connectd");
        closesocket(servfd);
        WSACleanup();
        return -1;
    }
   

      int sin_size= sizeof(struct sockaddr_storage);
        if((connfd=accept(servfd,(struct sockaddr*)&clientaddr,&sin_size)) == SOCKET_ERROR){
            perror("Accepting");
            closesocket(servfd);
            WSACleanup();
            return -1;
        }

    char s[INET6_ADDRSTRLEN];

    inet_ntop(clientaddr.ss_family,get_in_addr((struct sockaddr*)&clientaddr),s,sizeof s);
    printf("server : got connection with %s on port %d\n",s,ntohs(((struct sockaddr_in*)&clientaddr)->sin_port));

    char filename[MAX_BUFFER];
    if(recv(connfd,filename,MAX_BUFFER,0) == -1){
        perror("recieving data");
        WSACleanup();
        closesocket(connfd);
        return -1;
    }

    FILE* fp=fopen(filename,"wb");

    receive_file(connfd,fp);

    printf("received succesfully %ld bytes\n",totale);
    closesocket(connfd);
    WSACleanup();
    fclose(fp);
    return 0;

    

}

void receive_file(SOCKET s,FILE* fp){

    size_t n;
    char readline[MAX_LINE] = {0};
     if(!fp){
        perror("File open error");
        exit(-1);
    }

    while((n = recv(s,readline,MAX_LINE,0)) > 0){
        if(n == -1){
            perror("Receive File Error");
            exit(-1);
        }
        totale += n;
        if(fwrite(readline,sizeof(char), n , fp) != n){
            perror("Write File Error");
            exit(-1);
        }
        memset(readline,0,sizeof(readline));
    }
}

