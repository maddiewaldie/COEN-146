//
//  Behnam Dezfouli
//  CSEN, Santa Clara University
//

// This program implements a client that connects to a server and transfers the bytes read from a file over the established connection
//
// The input arguments are as follows:
// argv[1]: Sever's IP address
// argv[2]: Server's port number
// argv[3]: Source file
// argv[4]: Destination file at the server side which includes a copy of the source file

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SIZE 10

int main(int argc, char *argv[]) {
    int sockfd = 0;                 // socket descriptor
    char net_buff[SIZE];            // to hold the bytes read from the socket
    char file_buff[SIZE];           // to hold the bytes read from the source file
    struct sockaddr_in serv_addr;   // server address structure
    int net_bytes_read;             // number of bytes received over the socket
    FILE *source_file;              // pointer to the source file
    
    if (argc < 5) {
        printf("Usage: ./%s <server ip address> <server port number> <source file>  <destination file>\n", argv[0]);
        return 1;
    }
    
    // Make sure initial values are zero
    memset(net_buff, '0', sizeof(net_buff));
    memset(file_buff, '0', sizeof(file_buff));
    memset(&serv_addr, 0, sizeof(serv_addr));

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Initialize server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open the source file for reading
    source_file = fopen(argv[3], "rb");
    if (!source_file) {
        perror("Input file opening failed");
        return 1;
    }

    // Read and send data to the server in chunks
    ssize_t bytes_read;
    while ((bytes_read = fread(file_buff, 1, SIZE, source_file)) > 0) {
        send(sockfd, file_buff, bytes_read, 0);
    }

    // Close the input file and the socket
    fclose(source_file);
    close(sockfd);

    printf("File %s sent to the server\n", argv[3]);
    return 0;
}
