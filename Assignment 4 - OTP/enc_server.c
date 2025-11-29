#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//a custom error function to report issues.
void report_error(const char *msg) 
{
    perror(msg);
    exit(1);
}

//function to map characters 'A'-'Z' and ' ' to integers 0-26.
int char_to_integer(char c) 
{
    if (c == ' ') return 26;
    if (c >= 'A' && c <= 'Z') return c - 'A';
    report_error("Server: ERROR, invalid character in input");
    return -1; // Should not be reached
}

//function to map integers 0-26 back to characters 'A'-'Z' and ' '.
char integer_to_char(int i) 
{
    if (i == 26) return ' ';
    return 'A' + i;
}

//main encryption logic.
char *encrypt_data(char *message, char *key) 
{
    int msg_len = strlen(message);
    int key_len = strlen(key);

    if (key_len < msg_len) 
    {
        fprintf(stderr, "Error: key is too short!\n");
        exit(1);
    }

    char *encrypted_text = malloc(sizeof(char) * (msg_len + 1));
    if (encrypted_text == NULL) 
    {
        report_error("Server: Memory allocation error");
    }

    for (int i = 0; i < msg_len; i++) 
    {
        int msg_val = char_to_integer(message[i]);
        int key_val = char_to_integer(key[i]);
        int encrypted_val = (msg_val + key_val) % 27;
        encrypted_text[i] = integer_to_char(encrypted_val);
    }
    encrypted_text[msg_len] = '\0';
    return encrypted_text;
}

int main(int argc, const char *argv[]) 
{
    //validate command-line arguments.
    if (argc < 2) 
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    int listening_socket, connected_socket, server_port;
    socklen_t client_address_size;
    struct sockaddr_in server_addr, client_addr;

    memset((char *)&server_addr, '\0', sizeof(server_addr));
    server_port = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket < 0) 
    {
        report_error("Server: ERROR opening socket");
    }

    if (bind(listening_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        report_error("Server: ERROR on binding");
    }
    listen(listening_socket, 5);

    while (1) 
    {
        client_address_size = sizeof(client_addr);
        connected_socket = accept(listening_socket, (struct sockaddr *)&client_addr, &client_address_size);
        if (connected_socket < 0) 
        {
            report_error("Server: ERROR on accept");
        }

        pid_t child_pid = fork();
        if (child_pid < 0) 
        {
            report_error("Server: FORK ERROR");
        }

        if (child_pid == 0) 
        {
            //child process handles the request.
            char buffer[1000001];
            char plaintext[1000001] = "";
            char key[1000001] = "";
            
            //handshake to send identifier and wait for client ack.
            send(connected_socket, "e", 1, 0);
            recv(connected_socket, buffer, sizeof(buffer), 0);

            //read plaintext
            memset(buffer, '\0', sizeof(buffer));
            recv(connected_socket, buffer, sizeof(buffer) - 1, 0);
            int plaintext_size = atoi(buffer);
            send(connected_socket, "ack", 3, 0);

            int bytes_read = 0;
            while (bytes_read < plaintext_size) 
            {
                memset(buffer, '\0', sizeof(buffer));
                int current_read = recv(connected_socket, buffer, sizeof(buffer) - 1, 0);
                if (current_read <= 0) break;
                strcat(plaintext, buffer);
                bytes_read += current_read;
            }
            send(connected_socket, "ack", 3, 0);

            //read key
            memset(buffer, '\0', sizeof(buffer));
            recv(connected_socket, buffer, sizeof(buffer) - 1, 0);
            int key_size = atoi(buffer);
            send(connected_socket, "ack", 3, 0);
            
            bytes_read = 0;
            while (bytes_read < key_size) 
            {
                memset(buffer, '\0', sizeof(buffer));
                int current_read = recv(connected_socket, buffer, sizeof(buffer) - 1, 0);
                if (current_read <= 0) break;
                strcat(key, buffer);
                bytes_read += current_read;
            }
            send(connected_socket, "ack", 3, 0);

            char *ciphertext = encrypt_data(plaintext, key);
            send(connected_socket, ciphertext, strlen(ciphertext), 0);

            free(ciphertext);
            close(connected_socket);
            exit(0);
        } 
        else 
        {
            //parent process
            close(connected_socket);
        }
    }
    close(listening_socket);
    return 0;
}
