#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SRV_PORT    1335        // Слушающий порт сервера
#define SRV_ADDR    "127.0.0.1" // Адрес сервера
#define MSG_MAXLEN  256         // Максимальная длина передаваемого или
                                // получаемого сообщения

void client_reply(struct sockaddr_in addr_clt);

int main(int argc, void *argv[])
{
    int                 sock;           // Слушающий сокет
    struct sockaddr_in  addr_srv;       // Данные сервера
    struct sockaddr_in  addr_clt;       // Данные подключающегося клиента
    char                msg[MSG_MAXLEN];// Получаемое сообщение
    socklen_t           addr_size;      // Размер  struct sockaddr_in

    // Создаем слушающий сокет
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1){
        perror("socket");
        exit(-1);
    }
    
    // Именуем его
    addr_size = sizeof(addr_srv);
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_port = htons(SRV_PORT);
    addr_srv.sin_addr.s_addr = inet_addr(SRV_ADDR);
    if (bind(sock, (struct sockaddr *)&addr_srv, sizeof(addr_srv)) == -1){
        perror("bind");
        exit(-1);
    }

    // При подключении нового клиента запускаем его обслуживание в новом процессе
    while (1){
        recvfrom(sock, msg, MSG_MAXLEN, 0, (struct sockaddr *)&addr_clt, &addr_size);
        switch (fork()){
            case -1:
                perror("fork");
            break;

            case 0:
                client_reply(addr_clt);
            break;

            default:
                continue;
            break;
        }
        
    }

    close(sock);
    exit(0);
}

/*
 * Функция создает новый сокет, назначая для клиента новый порт и отвечает
 * на любое его сообщение текущим временем сервера.
 * 
 * @param addr_clt данные нового клиента
 */

void client_reply(struct sockaddr_in addr_clt)
{
    int                 sock;           // Новый сокет для клиента
    struct sockaddr_in  addr_srv;       // Данные сервера
    char                msg[MSG_MAXLEN];// Получаемое сообщение
    socklen_t           addr_size;      // Размер  struct sockaddr_in
    time_t utime;                       // Текущее время в микросекундах
    struct tm *tmp;                     // Абсолютное текущее время
    
    // Создаем новый сокет
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1){
        perror("socket");
        exit(-1);
    }
    // Именуем его, назначая свободный порт
    addr_size = sizeof(addr_srv);
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_port = 0;
    addr_srv.sin_addr.s_addr = inet_addr(SRV_ADDR);
    if (bind(sock, (struct sockaddr *)&addr_srv, sizeof(addr_srv)) == -1){
        perror("bind");
        exit(-1);
    }

    while (1)
    {
        // Формируем время в нужном формате и отправляем
        utime = time(NULL);
        tmp = localtime(&utime);
        strftime(msg, MSG_MAXLEN, "%H:%M:%S", tmp);
        sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr *)&addr_clt, addr_size);

        // Принимаем новый запрос
        recvfrom(sock, msg, MSG_MAXLEN, 0, (struct sockaddr *)&addr_clt, &addr_size);
        if (!strcmp(msg, "disconn"))
            break;
    }
    close(sock);    
}
