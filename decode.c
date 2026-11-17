#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include <unistd.h>

// oping the setgo.bmp file and output.txt file for decoding and strong the secret message
Status open_file(char *argv[], DecodeInfo *decode)
{

    decode->fptr_stego_image = fopen(argv[2], "r"); // opning the setgo.bmp file in read mode

    if (decode->fptr_stego_image == NULL)
    {

        perror("");
        return d_failure;
    }

    return d_success;
}

// reading and checking the argument on cla 
Status read_and_check_argument(char *argv[], DecodeInfo *decode) 
{

    if (strstr(argv[2], ".bmp") == NULL) // if argument 2 is .bmp or not
    {

        return d_failure;
    }

    if (argv[3] != NULL) // if argument 3 is present or not is present then store in variable if not store "output" in variable
    {

        strcpy(decode->output_fname, argv[3]);
    }
    else
    {

        strcpy(decode->output_fname, "output");
    }

    return d_success;
}

// calling the do decoding function which contain all the decoding functions

Status do_decoding(char *argv[], EncodeInfo *encInfo, DecodeInfo *decode)
{

    if (open_file(argv, decode) == d_success) // opning the files like stego.bmp output.txt
    {
        printf("Enter the magic string : ");
        scanf(" %s", decode->magic_string);
        printf("\n");

        if (check_magic_string(decode) == d_success)
        {

            printf("Decoded the Magic string \n\n");
            sleep(1);
        }
        else
        {
            printf("Magic string not found\n\n");
            return d_failure;
        }

        if (decode_secret_file_extn_size(decode) == d_success)
        {
            printf("Decoded the secret file extention size\n\n");
            sleep(1);
        }
        else
        {

            return d_failure;
        }
        if (decode_secret_file_extn(decode) == d_success)
        {
            printf("Decoded the secret file extention\n\n");
            sleep(1);
        }
        else
        {

            return d_failure;
        }
        if (decode_secret_file_size(decode) == d_success)
        {
            printf("Decoded the secret file size\n\n");
            sleep(1);
        }
        else
        {

            return d_failure;
        }
        if (decode_secret_file_data(decode) == d_success)
        {
            printf("Decoded the secret file data\n\n");
            sleep(1);
        }
        else
        {

            return d_failure;
        }

        printf("Successfully Decoded the message\n\n");
    }
}

// decoding the magic string the comparing with user  magic string
Status check_magic_string(DecodeInfo *decode)
{

    int size = strlen(decode->magic_string); // getting the length of user magic string

    char buffer[100] = {'\0'};

    fseek(decode->fptr_stego_image, 54, SEEK_SET); // skpping the 54 bytes of header

    decode_byte_to_lsb(size, decode->fptr_stego_image, buffer); // calling the function for decoding the magic string

    if (strcmp(buffer, decode->magic_string) == 0) // comparing the user magic string and  decoded magic string 
    {

        return d_success;  
    }
    else
    {

        return d_failure;
    }
}

// decoding the file extention size
Status decode_secret_file_extn_size(DecodeInfo *decode)
{

    decode_size_to_lsb(decode, &decode->extn_file_size); // decoding the size of extention using this function
    return d_success;
}

// decoding the characters from stego.bmp
Status decode_byte_to_lsb(int size, FILE *fptr_to_stego_image, char *buffer)
{

    char beautiful;

    for (int j = 0; j < size; j++)
    {
        for (int i = 0; i < 8; i++)
        {  

            fread(&beautiful, 1, 1, fptr_to_stego_image);

            buffer[j] = buffer[j] | ((beautiful & 1) << i);
        }
    }
}

// decoding the integer values from stego.bmp
Status decode_size_to_lsb(DecodeInfo *decode, int *dest)
{
    int buffer;

    int size = 0;
    for (int i = 0; i < 32; i++)
    {

        fread(&buffer, 1, 1, decode->fptr_stego_image);

        size = size | (buffer & 1) << i;
    }

    *dest = size;
}

// decoding the secret file extention 
Status decode_secret_file_extn(DecodeInfo *decode)
{

    char buffer[100] = {'\0'};

    // printf("%d\n",decode->extn_file_size);

    decode_byte_to_lsb(decode->extn_file_size, decode->fptr_stego_image, buffer); //decoding the extention file using this function
    decode->file_extn = buffer;

    return d_success;
}

// decoding the secret file size
Status decode_secret_file_size(DecodeInfo *decode)
{

    decode_size_to_lsb(decode, &decode->secret_file_size);// decoding the size of file using this function

    return d_success;
}

// decoding the secret data form stego.bmp

Status decode_secret_file_data(DecodeInfo *decode)
{
    char buffer[100] = {'\0'};
    decode_byte_to_lsb(decode->secret_file_size, decode->fptr_stego_image, buffer); // calling this function for decoding the data from stego.bmp

    strcpy(decode->output_fname, strcat(decode->output_fname, ".txt")); // concating the .txt to output to store the secret data

    FILE *fptr = fopen(decode->output_fname, "w"); // opning the output.txt file in write mode

    if (fptr == NULL)
    {

        perror("");
        return d_failure;
    }

    fwrite(buffer, 1, decode->secret_file_size, fptr); // writting the decoded secret data into output.txt

    fclose(fptr);

    return d_success;
}
