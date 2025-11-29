#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//check if a string contains only digits.
int check_if_numeric(const char *input_string) 
{
    if (input_string == NULL || *input_string == '\0') 
    {
        return -1;
    }
    for (int i = 0; input_string[i] != '\0'; i++) 
    {
        if (input_string[i] < '0' || input_string[i] > '9') 
        {
            return -1;
        }
    }
    return 0;
}

//function to generate and print the key.
void generate_key(int length) 
{
    const char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; //character bank
    for (int i = 0; i < length; i++) 
    {
        int random_char_index = rand() % 27;
        printf("%c", char_set[random_char_index]);
    }
    printf("\n");
}

int main(int argc, const char *argv[]) 
{
    //initialize the random number generator.
    srand(time(NULL));

    //ensure proper argument usage.
    if (argc != 2) 
    {
        fprintf(stderr, "Please run command with one arg: keygen [keylength]\n");
        exit(0);
    }
    
    //validate the key length argument.
    if (check_if_numeric(argv[1]) != 0) 
    {
        fprintf(stderr, "Please enter an integer for the second argument.\n");
        exit(0);
    }

    int key_length = atoi(argv[1]);
    generate_key(key_length);

    return 0;
}
