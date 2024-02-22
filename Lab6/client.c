// SCU COEN 146
//
// This program implements the client side of the stop & wait protocol
// where the client transfers a file to the server
//
//
// For the stop and wait protocol, the assumptions are:
//      -- packet corruption, and packet loss

#include "tfv1.h"

// Global variables
int state = 0; // Only two states: 0 and 1
int sock;
struct sockaddr_in serverAddr;
socklen_t addr_size;

// Function prototypes
int main(int, char *[]);
void my_send(char *, int);
void recv_ack(PACKET *, int);
int calc_checksum(char *, int);

int main(int argc, char *argv[]) {
    char buff[SIZE];
    FILE *fp; // File to be sent to the server
    int r;

    if (argc != 5) {
        printf("Usage: ./client port IP source_file dest_file\n");
        return 1;
    }

    // Configure the server address
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_family = AF_INET;


    // Create a UDP socket
    // UDP alone (no IP)
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
    printf("UDP socket has been created!\n");

    if ((fp = fopen(argv[3], "rb")) == NULL) {
        perror("fopen error");
        return 1;
    }
    printf("Source file opened successfully!\n");

    printf("Sending file name...");
    my_send(argv[4], strlen(argv[4]));
    printf("Sent File name\n");

    printf("Now sending source file contents...\n");
    while ((r = fread(&buff, sizeof(char), SIZE, fp)) > 0) {
        my_send(buff, r);
    }
    printf("Done!\n");

    // After transmitting the file, a packet with no data (len = 0) is sent to
    // notify the receiver that the file transfer has completed
    printf("Informing the server about the completion of file transmission...\n");

    my_send(buff, 0); // Don't really care what to send, as long as len = 0
    printf("Done!\n");

    fclose(fp);
    close(sock);

    return 0;
}

// Sends data (of size nbbytes) unreliably to the sender
void my_send(char *data, int nbytes) {
    PACKET buf;
    int r;
    int cs;

    // Default checksum value is 0
    buf.header.checksum = 0;
    buf.header.len = nbytes;
    buf.header.seq_ack = state;

    printf("Sequence number: %d\n", buf.header.seq_ack);

    strcat(data, "\0");
    printf("Data: %s\n", data);
    strcpy(buf.data, data);

    // Simulating an erroneous channel... corruption and loss
    // The probability of a correct checksum is 80%
    r = rand() % 10;
    if (r <= 8) {
        buf.header.checksum = calc_checksum((char *)&buf, sizeof(HEADER) + buf.header.len);
    } else {
        printf("\nPacket got corrupted on the way!\n");
    }

    cs = state;

    printf("\tChecksum: %d\n", buf.header.checksum);
    printf("\tData: %s\n", buf.data);

    // The probability of no packet loss is 80%
    r = rand() % 10;
    if (r <= 8) {
        sendto(sock, &buf, sizeof(HEADER) + nbytes, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    } else {
        printf("\nPacket got lost along the way!\n");
    }

    // Wait for acknowledgment
    recv_ack(&buf, nbytes);

    // If acknowledgment is not correctly received, retransmit data
    if (cs == state) {
        my_send(data, nbytes);
    }
}

// Wait for acknowledgment
void recv_ack(PACKET *buf, int nbytes) {
    HEADER receive_buf;
    int cs;
    int r;
    struct timeval tv;
    int rv;
    int counter = 0;
    fd_set readfds;
    FD_ZERO(&readfds); // Clear an fd_set
    FD_SET(sock, &readfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    rv = select(sock + 1, &readfds, NULL, NULL, &tv);

    if (rv == 1) {
        // Data is available to read
        recvfrom(sock, &receive_buf, sizeof(HEADER), 0, (struct sockaddr *)&serverAddr, &addr_size);
        cs = receive_buf.checksum;
        receive_buf.checksum = 0;

        // Recalculate checksum of the received ack packet
        receive_buf.checksum = calc_checksum((char *)&receive_buf, sizeof(HEADER));

        printf("\tChecksum: %d\n", receive_buf.checksum);
        printf("\tSequence: %d\n", receive_buf.seq_ack);
        printf("\tCurrent state: %d\n", state);

        if (cs == receive_buf.checksum && state == receive_buf.seq_ack) {
            // Good acknowledgment received
            state = !state;
            return; 
        } else {
            // Bad acknowledgment received, handle retransmission
            printf("\nReceived a bad packet\n");
            return;
        }
    } else if (rv == 0) {
        // Timeout occurred, handle retransmission
        printf("Timeout\n");
        if (nbytes == 0) {
            counter++;
            if (counter >= 3) {
                return; // Maximum retries reached
            }  
        }
    }
}

// Calculate checksum using XOR
int calc_checksum(char *buf, int tbytes) {
    int i;
    char cs = 0;
    char *p;

    p = buf;
    cs = *p;

    for (i = 1; i < tbytes; i++) {
        cs ^= *p;
        p++;
    }
    return (int)cs;
}
