#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

//侦听地址和端口
const char *host = "0.0.0.0";
int port = 80;

int main() {
    //创建主Socket
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    if(sockid <= 0) {
        ::perror("Create Socket Failed!");
        return -1;
    }

    //绑定侦听信息到主Socket
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(host);

    socklen_t socklen = sizeof(struct sockaddr_in);
    if(bind(sockid, (struct sockaddr*)&saddr, socklen) <= 0) {
        perror("Socket Bind Failed!");
        return -1;
    }

    //主Socket启动侦听
    if(listen(sockid, 5) <= 0) {
        ::perror("Socket Listen Failed!");
        close(sockid);
        return -1;
    }

    //开始接受外部主机的连接和数据
    //先接受(阻塞)，有连接后自动建立通讯
    while(1) {
        //创建与客户端通讯的次Socket后主动先接受连接
        int n_sockid;
        struct sockaddr_in rmt;
        n_sockid = accept(sockid, (struct sockaddr*)&rmt, &socklen);
        if(n_sockid <= 0) {
            perror("Socket Accept Failed!");
            continue;
        }

        //开始接收数据
        while(1) {
            char Buffer1 = {0};
            int sockret = recv(n_sockid, Buffer1, 1023, 0);
            if(sockret < 0) {
                perror("Socket RX Failed!");
                continue;
            }
            printf("Client %s[%d]: ", inet_ntoa(rmt.sin_addr), ntohs(rmt.sin_port), Buffer1);

            memset(Buffer1, 0x00, 1024);
            scanf("%s", Buffer1);
            //发送数据回去
            //send(n_sockid, Buffer1, sizeof(Buffer1), 0);
        }
        //次Socket关闭
        close(n_sockid);
    }
    //主Socket关闭
    close(sockid);
    return 0;
}
