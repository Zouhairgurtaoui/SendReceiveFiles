#include <libgen.h>
#include "transfer.h"

void send_file(SOCKET ,FILE* );
ssize_t totale = 0;

int main(int argc,char* argv[]){


    if(argc != 3){
        fprintf(stderr,"Error usage:./filename <IP-addr> <File Path>\n");
        exit(-1);
    }

    SOCKET clientfd;
    WSADATA wsadata;
    WORD wVersionRequested = MAKEWORD(2,2);
    int wsaerr;
    wsaerr = WSAStartup(wVersionRequested,&wsadata);
    if(wsaerr != 0){
        fprintf(stderr,"Error loading dll %s\n",WSAGetLastError());
        WSACleanup();
        return -1;
    }

    struct addrinfo hints,*servinfo;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rv = getaddrinfo(argv[1],SERVERPORT,&hints,&servinfo);

    if(rv != 0){
        fprintf(stderr,"getaddrinfo : %s\n",gai_strerror(rv));
        WSACleanup();
        exit(-1);
    }

    if((clientfd=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol))== SOCKET_ERROR){
        perror("Creating Socket");
        WSACleanup();
        exit(-1);
    }

    if(connect(clientfd,servinfo->ai_addr,sizeof(*servinfo)) == SOCKET_ERROR){
        perror("Connecting");
         printf("%d",WSAGetLastError());
        WSACleanup();
        closesocket(clientfd);
        exit(-1);
    }
    char* filename = basename(argv[2]);

    char buff[MAX_LINE] = {0};
    
    strncpy_s(buff, MAX_LINE,filename, strlen(filename));

    if(send(clientfd,buff,MAX_LINE,0)==SOCKET_ERROR){
        perror("Sending Data");
        closesocket(clientfd);
        WSACleanup();
        exit(-1);
    }

    FILE* fp = fopen(argv[2],"rb");

    send_file(clientfd,fp);

    printf("File Sent Successfully\n sent %ld\n",totale);
    closesocket(clientfd);
    WSACleanup();
    fclose(fp);
    
  
   return 0;

}

void send_file(SOCKET s,FILE* fp){

    int n;
    char sendline[MAX_LINE] ={0};

    if(!fp){
        perror("File open error");
        exit(-1);
    }

    while((n = fread(sendline,sizeof(char),MAX_LINE,fp)) > 0 ){
        

         if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(-1);
        }
        totale += n;
        
        if((n = send(s,sendline,n,0)) == SOCKET_ERROR){
            perror("Sending Data");
            exit(-1);
        }
        memset(sendline,0,MAX_LINE);
    }
}