#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SRV_PORT    1336        // Слушающий порт сервера
#define SRV_ADDR    "127.0.0.1" // Адрес сервера
#define MSG_MAXLEN  256         // Максимальная длина передаваемого или
                                // получаемого сообщения

int main(int argc, void *argv[])
{
    int                 sock;           // Сокет клиента    
    struct sockaddr_in  addr_srv;       // Данные сервера
    char                msg[MSG_MAXLEN];// Отправляемое / получаемое сообщение
    socklen_t           addr_size;      // Размер  struct sockaddr_in

    // Создаем сокет клиента
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1){
        perror("socket");
        exit(-1);
    }
    
    // Заполняем данные сервера
    addr_size = sizeof(addr_srv);
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_port = htons(SRV_PORT);
    addr_srv.sin_addr.s_addr = inet_addr(SRV_ADDR);
    
    while (1){
        // Считываем и отправляем
        printf("send: ");
        scanf("%s", msg);
        sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr *)&addr_srv, addr_size);
        if (!strcmp(msg, "disconn"))
            break;
        // Получаем ответ от сервера
        if (recvfrom(sock, msg, MSG_MAXLEN, 0, (struct sockaddr *)&addr_srv, &addr_size) == -1)
            break;
        printf("recv: %s\n", msg);
    }

    close(sock);
    exit(0);
}
