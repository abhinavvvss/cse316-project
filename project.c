#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KEY 0xAB  // XOR key for encryption (simplified)
#define MAX_FILENAME 256
#define STORAGE_DIR "storage/"
#define VERSION_DIR "versions/"

// Utility to create directories if not exist (Linux/macOS)
void ensure_directories() {
    system("mkdir -p storage");
    system("mkdir -p versions");
}

// XOR encryption/decryption
void xor_encrypt_decrypt(char *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= KEY;
    }
}

// Save file with encryption
void upload_file(const char *filepath) {
    FILE *input = fopen(filepath, "rb");
    if (!input) {
        perror("Cannot open input file");
        return;
    }

    char filename[MAX_FILENAME];
    printf("Enter a name to save this file as: ");
    scanf("%s", filename);

    char fullpath[MAX_FILENAME];
    snprintf(fullpath, sizeof(fullpath), STORAGE_DIR"%s", filename);

    FILE *output = fopen(fullpath, "wb");
    if (!output) {
        perror("Cannot open output file");
        fclose(input);
        return;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        xor_encrypt_decrypt(buffer, bytesRead);
        fwrite(buffer, 1, bytesRead, output);
    }

    fclose(input);
    fclose(output);
    printf("File uploaded and encrypted as: %s\n", filename);
}

// Decrypt and save file
void download_file(const char *filename) {
    char fullpath[MAX_FILENAME];
    snprintf(fullpath, sizeof(fullpath), STORAGE_DIR"%s", filename);

    FILE *input = fopen(fullpath, "rb");
    if (!input) {
        perror("File not found");
        return;
    }

    char outputname[MAX_FILENAME];
    printf("Enter output path to save decrypted file: ");
    scanf("%s", outputname);

    FILE *output = fopen(outputname, "wb");
    if (!output) {
        perror("Cannot write output file");
        fclose(input);
        return;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        xor_encrypt_decrypt(buffer, bytesRead);
        fwrite(buffer, 1, bytesRead, output);
    }

    fclose(input);
    fclose(output);
    printf("File decrypted and saved as: %s\n", outputname);
}

// List stored files
void list_files() {
    printf("Files in storage:\n");
    system("ls storage");
}

// Delete file
void delete_file(const char *filename) {
    char command[MAX_FILENAME + 20];
    snprintf(command, sizeof(command), "rm storage/%s", filename);
    int result = system(command);
    if (result == 0) {
        printf("File deleted: %s\n", filename);
    } else {
        printf("File not found.\n");
    }
}

// Version file (copy with timestamp)
void version_file(const char *filename) {
    char source[MAX_FILENAME], dest[MAX_FILENAME];
    snprintf(source, sizeof(source), STORAGE_DIR"%s", filename);

    FILE *src = fopen(source, "rb");
    if (!src) {
        printf("File not found: %s\n", filename);
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "_%Y%m%d%H%M%S", tm_info);

    snprintf(dest, sizeof(dest), VERSION_DIR"%s%s", filename, timestamp);
    FILE *dst = fopen(dest, "wb");

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytesRead, dst);
    }

    fclose(src);
    fclose(dst);
    printf("Version created: %s\n", dest);
}

// Main interface
void menu() {
    int choice;
    char filename[MAX_FILENAME];

    do {
        printf("\n=== Secure File Manager ===\n");
        printf("1. Upload File\n");
        printf("2. Download File\n");
        printf("3. List Files\n");
        printf("4. Delete File\n");
        printf("5. Create Version\n");
        printf("0. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);
        getchar();  // flush newline

        switch (choice) {
            case 1:
                {
                    char path[MAX_FILENAME];
                    printf("Enter file path to upload: ");
                    scanf("%s", path);
                    upload_file(path);
                }
                break;
            case 2:
                printf("Enter filename to download: ");
                scanf("%s", filename);
                download_file(filename);
                break;
            case 3:
                list_files();
                break;
            case 4:
                printf("Enter filename to delete: ");
                scanf("%s", filename);
                delete_file(filename);
                break;
            case 5:
                printf("Enter filename to version: ");
                scanf("%s", filename);
                version_file(filename);
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main() {
    ensure_directories();
    menu();
    return 0;
}
