#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>

#define COMMAND_BUFF 15000
#define SOCK_READ_BUFF 4096
#define REPLY_BUFF 20000
#define FILE_READ 1024 // Increased the buffer size for file reading
#define HTML_FILE "index.html"

int socket_fd = 0;
int connection_fd = 0;

void INThandler(int sig) {
    char input;
    signal(sig, SIG_IGN);
    printf("Did you hit Ctrl-C?\nDo you want to quit? [y/n] ");
    input = getchar();
    if (input == 'y' || input == 'Y') {
        printf("Server is shutting down...\n");
        if (connection_fd > 0) {
            shutdown(connection_fd, SHUT_RDWR);
            close(connection_fd);
        }
        if (socket_fd > 0) {
            close(socket_fd);
        }
        exit(0);
    } else {
        signal(SIGINT, INThandler);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, INThandler);

    int net_bytes_read;
    struct sockaddr_in serv_addr;
    char net_buff[SOCK_READ_BUFF];
    char message_buff[REPLY_BUFF];
    char file_buff[FILE_READ];
    FILE *source_file;

    if (argc < 2) {
        printf("Port number not specified!\n");
        return 1;
    }

    // Set up the server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: Could not create socket!\n");
        return 1;
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind the socket
    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed!");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (listen(socket_fd, 1) < 0) {
            perror("Listen failed!");
            exit(EXIT_FAILURE);
        }

        printf("Listening to incoming connections...\n");

        connection_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
        if (connection_fd < 0) {
            perror("Accept failed!");
            exit(EXIT_FAILURE);
        }

        printf("Incoming connection accepted!\n");

        unsigned int option = 0;
        printf("1: System network configuration\n2: Regular HTTP server\n");
        scanf("%u", &option);

        if (option == 1) {
            printf("System network configuration (using ifconfig)...\n");

            FILE *system_info = popen("ifconfig", "r"); // Use popen to run the command and capture its output

            if (system_info) {
                memset(message_buff, 0, sizeof(message_buff));
                size_t bytes_read = fread(message_buff, 1, REPLY_BUFF, system_info);

                if (bytes_read > 0) {
                    // Send the HTTP response header
                    send(connection_fd, "HTTP/1.1 200 OK\r\nServer: Maddie's Server\nContent-Type: text/plain\r\n\r\n", 45, 0);
                    send(connection_fd, message_buff, bytes_read, 0); // Send the result to the client
                }

                pclose(system_info); // Close the command output stream
            }

            shutdown(connection_fd, SHUT_RDWR);
            close(connection_fd);
        } else if (option == 2) {
            // To get the size of html file
            struct stat sbuf; /* file status */

            // Make sure the file exists
            // HTML_FILE is index.html and is statically defined
            if (stat(HTML_FILE, &sbuf) < 0) {
                send(connection_fd, "HTTP/1.1 404 Not Found\r\n\r\n", 26, 0);
                send(connection_fd, "Error: File not found", 20, 0);
            } else {
                source_file = fopen(HTML_FILE, "rb");

                if (source_file) {
                    fseek(source_file, 0, SEEK_SET); // Seek to the beginning of the file

                    // Send the HTTP response header
                    send(connection_fd, "HTTP/1.1 200 OK\r\nServer: Maddie's Server\nContent-Type: text/html\r\n\r\n", 45, 0);

                    size_t read_size;

                    // Read the file in chunks and send it to the client
                    while ((read_size = fread(file_buff, 1, FILE_READ, source_file)) > 0) {
                        send(connection_fd, file_buff, read_size, 0);
                    }

                    fclose(source_file);
                } else {
                    printf("Error: could not open the source file!\n");
                }

                shutdown(connection_fd, SHUT_RDWR);
                close(connection_fd);
            }
        }
    }
    close(socket_fd);
    return 0;
}
