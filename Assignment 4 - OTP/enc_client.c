#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void handle_error(const char *message) 
{
    perror(message);
    exit(1);
}

//helper function to read a file's content and size.
void get_file_data(const char *filename, char **content_buffer, long *size) 
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    rewind(fp);

    *content_buffer = (char *)malloc((*size + 1) * sizeof(char));
    if (*content_buffer == NULL) 
    {
        handle_error("Memory allocation failed");
    }

    fgets(*content_buffer, *size + 1, fp);
    fclose(fp);

    //replace newline with null terminator and adjust size
    int length = strlen(*content_buffer);
    if (length > 0 && (*content_buffer)[length - 1] == '\n') 
    {
        (*content_buffer)[length - 1] = '\0';
        *size = length - 1;
    }
}

int main(int argc, const char *argv[]) 
{
    //validate command-line arguments.
    if (argc != 4) 
    {
        fprintf(stderr, "Please run command with three args: otp_enc [plaintext] [key] [port]\n");
        exit(1);
    }

    const char *plaintext_filename = argv[1];
    const char *key_filename = argv[2];
    int port_number = atoi(argv[3]);

    if (access(plaintext_filename, F_OK) == -1) 
    {
        fprintf(stderr, "Error: file \"%s\" does not exist!\n", plaintext_filename);
        exit(1);
    }
    if (access(key_filename, F_OK) == -1) 
    {
        fprintf(stderr, "Error: file \"%s\" does not exist!\n", key_filename);
        exit(1);
    }
    
    //setup network communication
    int comm_socket;
    struct sockaddr_in server_address_info;
    struct hostent *server_host_entry;

    memset((char *)&server_address_info, '\0', sizeof(server_address_info));
    server_address_info.sin_family = AF_INET;
    server_address_info.sin_port = htons(port_number);
    server_host_entry = gethostbyname("localhost");
    if (server_host_entry == NULL) 
    {
        fprintf(stderr, "Client: ERROR, no such host\n");
        exit(1);
    }
    memcpy((char *)&server_address_info.sin_addr.s_addr, (char *)server_host_entry->h_addr, server_host_entry->h_length);

    comm_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (comm_socket < 0) 
    {
        handle_error("Client: ERROR opening socket");
    }

    if (connect(comm_socket, (struct sockaddr *)&server_address_info, sizeof(server_address_info)) < 0) 
    {
        fprintf(stderr, "Error: could not contact enc_server on port %d\n", port_number);
        exit(2);
    }

    char server_check[2];
    recv(comm_socket, server_check, 1, 0);
    if (server_check[0] != 'e') 
    {
        fprintf(stderr, "Error: could not contact enc_server on port %d\n", port_number);
        exit(2);
    }
    send(comm_socket, "ack", 3, 0);
    
    //file operations and data transfer ---
    char *plaintext_content, *key_content;
    long plaintext_size, key_size;
    get_file_data(plaintext_filename, &plaintext_content, &plaintext_size);
    get_file_data(key_filename, &key_content, &key_size);

    if (key_size < plaintext_size) 
    {
        fprintf(stderr, "Error: key '%s' is too short\n", key_filename);
        exit(1);
    }
    
    //send plaintext size and content
    char size_buffer[20];
    sprintf(size_buffer, "%ld", plaintext_size);
    send(comm_socket, size_buffer, strlen(size_buffer), 0);
    recv(comm_socket, size_buffer, 20, 0);

    send(comm_socket, plaintext_content, plaintext_size, 0);
    recv(comm_socket, size_buffer, 20, 0);

    //send key size and content
    sprintf(size_buffer, "%ld", key_size);
    send(comm_socket, size_buffer, strlen(size_buffer), 0);
    recv(comm_socket, size_buffer, 20, 0);

    send(comm_socket, key_content, key_size, 0);
    recv(comm_socket, size_buffer, 20, 0);
    
    //receive encrypted data
    long bytes_received = 0;
    while (bytes_received < plaintext_size) 
    {
        char buffer[100001] = {0};
        int current_read = recv(comm_socket, buffer, sizeof(buffer) - 1, 0);
        if (current_read < 0) handle_error("Client: ERROR reading from socket");
        printf("%s", buffer);
        bytes_received += current_read;
    }
    printf("\n");

    free(plaintext_content);
    free(key_content);
    close(comm_socket);

    return 0;
}
