#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_BUFFER_SIZE 1024
int splitFile(char *sourceFile, int numParts);

int joinFiles(char **partFiles, int numParts, char *outputFile);

// Function to handle errors
void handleError(const char *message);
int splitFile(char *sourceFile, int numParts) {
    // Open the source file
    FILE *source = fopen(sourceFile, "rb");
    if (source == NULL) {
        handleError("Failed to open source file");
        return 1;
    }

    fseek(source, 0, SEEK_END);
    long fileSize = ftell(source);
    fseek(source, 0, SEEK_SET);

    
    long partSize = fileSize / numParts;
    
    char buffer[MAX_BUFFER_SIZE];

    for (int i = 0; i < numParts; i++) {
        char partFileName[MAX_FILENAME_LENGTH];
        sprintf(partFileName, "%s.part%d", sourceFile, i + 1);

        FILE *part = fopen(partFileName, "wb");
        if (part == NULL) {
            handleError("Failed to create part file");
            fclose(source);
            return 1;
        }

        long bytesRead = 0;
        while (bytesRead < partSize) {
            size_t bytesToRead = (partSize - bytesRead < MAX_BUFFER_SIZE) ? (partSize - bytesRead) : MAX_BUFFER_SIZE;
            size_t bytesReadThisTime = fread(buffer, 1, bytesToRead, source);
            if (bytesReadThisTime == 0) {
                break;  
            }
            fwrite(buffer, 1, bytesReadThisTime, part);
            bytesRead += bytesReadThisTime;
        }

        fclose(part);
    }

    fclose(source);
    return 0;
}


int joinFiles(char **partFiles, int numParts, char *outputFile) {
    FILE *output = fopen(outputFile, "wb");
    if (output == NULL) {
        handleError("Failed to create output file");
        return 1;
    }

    char buffer[MAX_BUFFER_SIZE];

    for (int i = 0; i < numParts; i++) {
        FILE *part = fopen(partFiles[i], "rb");
        if (part == NULL) {
            handleError("Failed to open part file");
            fclose(output);
            return 1;
        }

        while (1) {
            size_t bytesRead = fread(buffer, 1, sizeof(buffer), part);
            if (bytesRead == 0) {
                break;
            }
            fwrite(buffer, 1, bytesRead, output);
        }

        fclose(part);
    }

    fclose(output);
    return 0;
}




void handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}



int main() {
    int choice;
    char sourceFile[MAX_FILENAME_LENGTH];
    char outputFile[MAX_FILENAME_LENGTH];
    int numParts;

    printf("File Splitter/Joiner\n");
    printf("1. Split a file\n");
    printf("2. Join files\n");
    printf("Enter your choice (1/2): ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter the source file name: ");
        scanf("%s", sourceFile);
        printf("Enter the number of parts: ");
        scanf("%d", &numParts);
        if (splitFile(sourceFile, numParts) == 0) {
            printf("File split successfully.\n");
        }
    } else if (choice == 2) {
        printf("Enter the number of parts: ");
        scanf("%d", &numParts);
        char **partFiles = (char **)malloc(numParts * sizeof(char *));
        for (int i = 0; i < numParts; i++) {
            partFiles[i] = (char *)malloc(MAX_FILENAME_LENGTH);
            printf("Enter part file %d name: ", i + 1);
            scanf("%s", partFiles[i]);
        }
        printf("Enter the output file name: ");
        scanf("%s", outputFile);
        if (joinFiles(partFiles, numParts, outputFile) == 0) {
            printf("Files joined successfully.\n");
        }
        for (int i = 0; i < numParts; i++) {
            free(partFiles[i]);
        }
        free(partFiles);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}



