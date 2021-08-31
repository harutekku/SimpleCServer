#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../include/diagnostics.h"

#define SIZE 1024
#define BACKLOG 10

int main(void) {
    //------------------------------------------------------------------------------------------
    // My data
    struct addrinfo         hints = { 0 };
    struct addrinfo*        res = NULL;
    int                     fd = -1;
    struct addrinfo*        i = NULL;
    char                    myIP[INET6_ADDRSTRLEN] = { 0 };
    int                     ec = 0;
    const size_t            timeSize = 20;
    char                    time[timeSize] = { 0 };

    // Their data
    struct sockaddr_storage theirAddress;
    socklen_t               theirAddressSize = sizeof(struct sockaddr_storage);
    int                     theirFd = -1;
    char                    theirIP[INET6_ADDRSTRLEN];
    char                    theirMessage[SIZE] = { 0 };
    
    //------------------------------------------------------------------------------------------
    // Prepare hints
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    //------------------------------------------------------------------------------------------
    // System calls
    if ((ec = getaddrinfo(NULL, "7590", &hints, &res)) != 0) {
        GetTime(time, timeSize);
        fprintf(stderr, "DEBUG: [%s] => getaddrinfo(): %s\n", time, gai_strerror(ec));
        return EXIT_FAILURE;
    }

    for (i = res; i != NULL; i = i->ai_next) {
        if ((fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            GetTime(time, timeSize);
            fprintf(stderr, "DEBUG: [%s] => socket(): %s\n", time, strerror(errno));
            continue;
        } else if (bind(fd, i->ai_addr, i->ai_addrlen) == -1) {
            close(fd);
            GetTime(time, timeSize);
            fprintf(stderr, "DEBUG: [%s] => bind(): %s\n", time, strerror(errno));
            continue;
        } else {
            if (i->ai_family == AF_INET) {
                inet_ntop(AF_INET, &((struct sockaddr_in*)&i->ai_addr)->sin_addr, myIP, INET6_ADDRSTRLEN);
                GetTime(time, timeSize);
                fprintf(stdout, "DEBUG: [%s] => Server active on IP: %s\n", time, myIP);
            } else {
                inet_ntop(AF_INET6, &((struct sockaddr_in6*)&i->ai_addr)->sin6_addr, myIP, INET6_ADDRSTRLEN);
                GetTime(time, timeSize);
                fprintf(stdout, "DEBUG: [%s] => Server active on IP: %s\n", time, myIP);
            }
            break;
        }
    }

    freeaddrinfo(res);

    if (!i) {
        GetTime(time, timeSize);
        fprintf(stderr, "DEBUG: [%s] => Unable to launch server\n", time);
        return EXIT_FAILURE;
    }

    if (listen(fd, BACKLOG) == -1) {
        GetTime(time, timeSize);
        fprintf(stderr, "DEBUG: [%s] => listen():%s\n", time, strerror(errno));
        return EXIT_FAILURE;
    }


    while (true) {
        theirFd = accept(fd, (struct sockaddr*)&theirAddress, &theirAddressSize);

        if (theirFd == -1) {
            GetTime(time, timeSize);
            fprintf(stderr, "DEBUG: [%s] => accept(): %s\n", time, strerror(errno));
            continue;
        }

        if (theirAddress.ss_family == AF_INET) {
            inet_ntop(AF_INET, &((struct sockaddr_in*)&theirAddress)->sin_addr, theirIP, INET6_ADDRSTRLEN);
            GetTime(time, timeSize);
            fprintf(stderr, "DEBUG: [%s] => IP %s connected\n", time, theirIP);
        } else {
            inet_ntop(AF_INET6, &((struct sockaddr_in6*)&theirAddress)->sin6_addr, theirIP, INET6_ADDRSTRLEN);
            GetTime(time, timeSize);
            fprintf(stderr, "DEBUG: [%s] => IP %s connected\n", time, theirIP);
        }
        size_t bytes = recv(theirFd, theirMessage, SIZE, 0);
        theirMessage[bytes] = '\0';

        GetTime(time, timeSize);
        fprintf(stderr, "DEBUG: [%s] => IP %s sent [%zu bytes]:\n%s\n", time, theirIP, bytes, theirMessage);

        close(theirFd);
    }


    close(fd);
    return EXIT_SUCCESS;
}
