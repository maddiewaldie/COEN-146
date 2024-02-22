//
//  Behnam Dezfouli
//  CSEN, Santa Clara University
//

// This program implements a server that accepts connection from a client and copies the received bytes to a file
//
// The input arguments are as follows:
// argv[1]: Sever's port number

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    char message[10] = "received!";   // message to be sent to the client when the destination file name is received
    int net_bytes_read;               // number of bytes received over the socket
    int socket_fd = 0;                // socket descriptor
    int connection_fd = 0;            // new connection descriptor
    struct sockaddr_in serv_addr;     // Address format structure
    char net_buff[1024];              // buffer to hold characters read from the socket
    FILE *dest_file;                  // pointer to the file that will include the received bytes over the socket
    
    if (argc < 2) { // Note: the name of the program is counted as an argument
        printf("Port number not specified!\n");
        return 1;
    }
    
    // Create a socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("Successfully created a socket.\n");

    // Initialize server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the network address and port
    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Binding failed");
        return 1;
    }

    printf("Successfully bound socket to the network address and port.\n");

    // Listen for incoming connections
    if (listen(socket_fd, 5) < 0) {
        perror("Listening failed");
        return 1;
    }

    printf("Successfully listening for incoming connections.\n");

    // Accept a client connection
    connection_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
    if (connection_fd < 0) {
        perror("Acceptance failed");
        return 1;
    }

    char dest_filename[1024]; // Assuming a maximum filename length of 1024 characters

    net_bytes_read = recv(connection_fd, dest_filename, sizeof(dest_filename), 0);
    if (net_bytes_read <= 0) {
        perror("Failed to receive the destination filename");
        return 1;
    }

    // Null-terminate the received filename
    dest_filename[net_bytes_read] = '\0';

    // Create and open the destination file for writing
    dest_file = fopen(dest_filename, "wb");
    if (!dest_file) {
        perror("Output file opening failed");
        return 1;
    }

    printf("Successfully received file name from the client.\n");

    // Send "received!" confirmation to the client
    write(connection_fd, message, strlen(message));
    printf("Successfully sent received confirmation to the client.\n");

    // Receive data from the client and write it to the destination file
    while ((net_bytes_read = read(connection_fd, net_buff, sizeof(net_buff))) > 0) {
        fwrite(net_buff, 1, net_bytes_read, dest_file);
    }
    printf("Successfully wrote data from the client to the destination file.\n");

    // Close the destination file and the connection
    fclose(dest_file);
    close(connection_fd);
    close(socket_fd);

    printf("File received and saved as %s\n", dest_filename);
    return 0;
}

