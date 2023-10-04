// Maddie Waldie
// COEN 146L - Lab 1
// 
// This project consists of building an application to copy a binary file to another file. We also measure file copy performance using various file sizes.
// The two operational modes are as follows:
// Option 1:
// Receive the name of source file from user; Receive the name of destination file from user; Copy the source file to the destination file; Write two separate functions for copying;
// One function that uses function call; Another that uses system call
// 
// Option 2:
// Receive the source file name from user; Receive maximum_file_size (in bytes) from user; Receive step_size (in bytes) from user; current_size = 0;
// While ( current_size < maximum_file_size )
// {
// Start time measurement;
// Create a file of current_size = current_size + step_size; Copy source file to destination file;
// Stop time measurement;
// Report time measurement;
// }

#include <stdio.h>
#include <stdlib.h> // For exit()
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> // for read and write

#define SIZE 1

// Function to copy files using function call
double copyFileFunctionCall(FILE *source, FILE *destination) {
    char buffer[SIZE];
    size_t bytes_read;
    double total_bytes = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        total_bytes += bytes_read;
        fwrite(buffer, 1, bytes_read, destination);
    }

    return total_bytes;
}

// Function to copy files using system call
double copyFileSystemCall(const char *source, const char *destination) {
    int source_fd, destination_fd;
    char buffer[1024];
    double total_bytes = 0;

    // Open the source file in read-only mode
    source_fd = open(source, O_RDONLY);
    if (source_fd == -1) {
        perror("Error opening source file");
        return -1.0;
    }

    // Open or create the destination file in write-only mode
    destination_fd = open(destination, O_WRONLY | O_CREAT, 0666);
    if (destination_fd == -1) {
        perror("Error opening destination file");
        close(source_fd);
        return -1.0;
    }

    // Read from the source file and write to the destination file
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(destination_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(source_fd);
            close(destination_fd);
            return -1.0;
        }
        total_bytes += bytes_written;
    }

    // Close both files
    close(source_fd);
    close(destination_fd);

    return total_bytes;
}

int main() {
    // Gather necessary information from the user
    int option;
    printf("Please select an option:\n");
    printf("1. Copy a file to another file.\n");
    printf("2. Measure the performance of file copy.\n");
    scanf("%d", &option);
    
    if ( option == 1 ) { // File copy
        // Gather necessary information from the user
        char sourceFileName[100], destinationFileName[100];
        printf("Please enter the name of the source file:\n");
        scanf("%s", &sourceFileName);
        printf("Please enter the name of the destination file:\n");
        scanf("%s", &destinationFileName);

        // Open the source file
        FILE *sourceFile = fopen(sourceFileName, "rb");
        if (sourceFile == NULL) {
            perror("Error opening file");
            return 1;
        } 

        // Open the destination file
        FILE *destinationFile = fopen(destinationFileName, "wb");
        if (destinationFile == NULL) {
            perror("Error opening file");
            fclose(sourceFile);
            return 1;
        } 

        // Determine the user's preferred method to copy the files
        int functionOrSystemCall;
        printf("Do you want to copy the files with a function call or a system call?\n");
        scanf("%d", &functionOrSystemCall);

        // Copy the source file to the destination file
        if (functionOrSystemCall == 1) {
            double total_bytes = copyFileFunctionCall(sourceFile, destinationFile);
            printf("File copy completed.\n");
        } else if (functionOrSystemCall == 2) {
            double total_bytes = copyFileSystemCall(sourceFileName, destinationFileName);
            printf("File copy completed.\n");
        }

        // Close both files
        fclose(sourceFile);
        fclose(destinationFile);
    }
    else if ( option == 2 ) { // File copy with performance measurement
        char destinationFileName[100];
        size_t maxFileSize, stepSize, currentSize = 0;
        
        // Gather necessary information from the user
        printf("Please enter the name of the destination file:\n");
        scanf("%s", &destinationFileName);
        printf("Enter maximum file size (in bytes): ");
        scanf("%zu", &maxFileSize);
        printf("Enter step size (in bytes): ");
        scanf("%zu", &stepSize);

        // Open the destination file
        FILE *destinationFile = fopen(destinationFileName, "wb");
        if (destinationFile == NULL) {
            perror("Error opening destination file");
            return 1;
        }

        while (currentSize < maxFileSize) { // Loop through until we hut the max file size
            FILE *sourceFile = fopen("file3.txt", "rb");
            size_t i;
            for (i = 0; i < currentSize; i++) {
                fputc('A', sourceFile);
            }

            // Start time measurement
            clock_t startTime = clock();
            double total_bytes = copyFileFunctionCall(sourceFile, destinationFile);

            // Stop time measurement
            clock_t endTime = clock();

            // Report time measurement
            double elapsedTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
            printf("File copy for size %zu bytes completed in %.6lf seconds (CPU time).\n", currentSize, elapsedTime);

            // Increment current_size
            currentSize += stepSize;
            fclose(sourceFile);
        }

        fclose(destinationFile);
    }
    else {
        printf("Invalid option!");
        return 0;
    }
    
    return 0;
}

