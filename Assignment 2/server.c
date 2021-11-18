// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    if (argv[0][2] == 'Y')
    {
        int fd = argv[0][1];
        valread = read(fd, buffer, 1024);
        printf("%s\n", buffer);
        send(fd, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    pid_t currentPid = fork();

    if (currentPid == 0)
    {
        printf("Child Running\n");
        int privilegeSuccess = setuid(65534);
        if (privilegeSuccess == -1)
        {
            printf("Setuid returned -1\n");
            return 0;
        }

        char fileDist_args[3];
        fileDist_args[0] = new_socket;
        fileDist_args[1] = new_socket;
        fileDist_args[2] = 'Y';
        execl(argv[0], fileDist_args, NULL);
    }

    else if (currentPid > 0)
    {
        wait(NULL);
        printf("Parent running\n");
    }
    else
    {
        perror("Unable to fork");
        _exit(2);
    }

    return 0;
}
