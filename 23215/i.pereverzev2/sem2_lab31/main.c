#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

#define PORT 8088
#define MAX_CLIENTS 64
#define BUFSIZE 1024

int main() {
    struct pollfd fds[MAX_CLIENTS + 1] = {0};
    char buffer[BUFSIZE] = {0};
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == 0) {
        perror("unable to create socket");
        exit(1);
    }
    struct sockaddr_in adrinf;
    memset(&adrinf, 0, sizeof adrinf);
    adrinf.sin_family = AF_INET;
    adrinf.sin_addr.s_addr = INADDR_ANY;
    adrinf.sin_port = htons(PORT);
    
    fds[MAX_CLIENTS].fd = socket_fd;
    fds[MAX_CLIENTS].events = POLLIN;

    if(bind(socket_fd, (struct sockaddr*)&adrinf, sizeof adrinf) < 0) {
        perror("unable to bind socket");
        exit(1);
    }

    int nfds = 1;
    listen(socket_fd, MAX_CLIENTS);
    while(1) {
        int poll_res = poll(fds, nfds, 0);
        if (poll_res < 0) {
            perror("unable to execute poll");
            exit(1);
        }
        if (poll_res == 0) {
            struct timespec slp;
            slp.tv_sec = 0;
            slp.tv_nsec = 100000000; // to not load device too much
            int result = nanosleep(&slp, NULL);
            continue;
        }
        // there are some ready connects
        if (fds[0].revents & POLLIN) {
            int accepted = accept(socket_fd, (struct sockaddr*)&adrinf, sizeof(adrinf));
            if (accepted < 0) {
                perror("unable to accept");
                exit(1);
            }

            int iter;
            for (iter = 1; iter <= MAX_CLIENTS; iter++) {
                if (fds[iter].fd == 0) {
                    fds[iter].fd = accepted;
                    fds[iter].events = POLLIN;
                    nfds++;
                    break;
                }
            }
            if (iter == MAX_CLIENTS) {
                fprintf(stderr, "unable to connect: limit of connections\n");
                close(socket_fd);
            }

            for(int i = 1; i <= MAX_CLIENTS; i++) {
                if(fds[i].revents & POLLIN) {
                    int code = read(fds[i].fd, buffer, sizeof(buffer));
                }
            }

        }
    }
    return 0;
}