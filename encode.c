#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>
#include <unistd.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height, header_size;

    fptr_image = fopen("beautiful.bmp", "rb"); // opening the .bmp file to get the width and height of that file

    fseek(fptr_image, 10, SEEK_SET); // skipping the 10 bytes

    fread(&header_size, 1, 4, fptr_image); // reading the size of the entire header

    fseek(fptr_image, 4, SEEK_CUR); // skipping the 4 bytes from currect position

    fread(&width, 1, 4, fptr_image); // reading the width

    fread(&height, 1, 4, fptr_image); // reading the height

    fclose(fptr_image);

    return width * height * 3; // returning the value (multipying by 3 because its RGB scale file for 3 bytes = 1 pixal)
}

uint get_file_size(FILE *fptr) // this function is for getting the size of secreat file
{

    fseek(fptr, 0, SEEK_END); // seeking the pointer at the end

    // return ftell(fptr) * 8;
    return ftell(fptr); // returning the value that is fptr is pointing
}
/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

// used for opning the files
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// checking the operation type
OperationType check_operation_type(char *argv[], int *argc)
{

    if (strcmp(argv[1], "-e") == 0) // if arg[1] is -e then it will return e_encode
    {

        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0) // if argv[1] is -d then it will return e_decode
    {

        return e_decode;
    }
    else
    {

        return e_unsupported; // if argv[1] dont have any thing then e_unsupported will be returned
    }
}

// checking if arguments are correct or not for encoding part

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc)
{
    // if argument count is 5 or 4 then it will execute the if block
    if (argc == 5 || argc == 4)
    {
        if ((strstr(argv[2], ".bmp") == NULL) || strstr(argv[3], ".txt") == NULL) // if argument count is 5 or 4 then it will check the argument 2 if it is .bmp or not and it also check the argument 3 is .txt or not
        {

            return e_failure; // if argument 2 and argument 3 is not present then return e_failure
        }

        if (argv[4] != NULL)
        {
            // if argument 4 is present then 
            if (strstr(argv[4], ".bmp") == NULL) // if argument 4 is not .bmp it will concate the argument 4 to .bmp
            {

                encInfo->stego_image_fname = strcat(argv[4], ".bmp"); 
            }
            else
            {

                encInfo->stego_image_fname = argv[4]; // if .bmp is present then stego_image_fname pointer will store the address of that argv[4]
            }
        }
        else
        {

            encInfo->stego_image_fname = "stegno.bmp"; // if argument 4 is not present then i am storing the stegno.bmp in fname
        }
    }
    else
    {
        // if argument count is greater than 5 and smaller than 4 then this code will be executed
        printf("Insufficient argument\n");

        printf("\nFormat should be :- \n");
        printf("\t./a.out -e source.bmp secret_file [destination.bmp]\n ");
        printf("\t./a.out -d destination.bmp [output]\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2]; // storing the beautiful.bmp name in pointer
    encInfo->secret_fname = argv[3]; // storing the secret.txt file name in pointer 

    encInfo->fptr_src_image = fopen(argv[2], "rb"); // opning the beautiful.bmp in read mode

    // Error checking
    if (encInfo->fptr_src_image == NULL)
    {

        return e_failure;
    }

    // validating the beautiful file is .bmp or not using the header byte
    fread(encInfo->image_data, 1, 2, encInfo->fptr_src_image);

    if (strcmp(encInfo->image_data, "BM") == 0)
    {
        fclose(encInfo->fptr_src_image);
        return e_success;
    }

    fclose(encInfo->fptr_src_image);
    return e_failure;
}

// checking the capacity of beautiful.bmp . Because we want store the magic string,size of secret extention,secret extention , secret file size,secret data
Status check_capacity(EncodeInfo *encInfo)
{

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // getting the beautiful.bmp capacity and stoing in a variable

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // getting the secret.txt file size and storing in a variable

    int magic_string = strlen(encInfo->magic_string) * 8; // calculating the magic length and multipying with 8 because we are stroring 1 bit in 1 byte so here 8 is bits 1bit needs 8 bits to encode

    int i = 0, char_count = 0;

    while (encInfo->secret_fname[i] != '.') // getting the extention
    {
        i++;
    }

    while (encInfo->secret_fname[i])
    {

        char_count++;
        i++;
    }

    char_count *= 8;

    uint capacity = char_count + encInfo->size_secret_file + magic_string + 64;

    if (capacity > encInfo->image_capacity) // checking the beautiful.bmp capacity 
    {

        return e_failure;
    }
    else
    {

        return e_success;
    }
}

// coping the header of beautiful.bmp into stego.bmp
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    
    char buffer[54];

    rewind(fptr_src_image);

    if (fptr_src_image == NULL)
    {

        return e_failure;
    }

    fread(buffer, 1, 54, fptr_src_image); // reading the 54 bytes from beautiful.bmp
    fwrite(buffer, 1, 54, fptr_dest_image); // writing the 54 bytes into stego.bmp

    return e_success;
}

// encoding the magic string 
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{

    int size = strlen(magic_string); // getting the magic string length


    if (encode_data_to_image(magic_string, size, encInfo) == e_success) // calling the data_to_image function and passing the magic_stirng, length of magic string and structure
    {

        return e_success;
    }
    else
    {

        return e_failure;
    }
}


Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    // running the loop till string length
    for (int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, 1, 8, encInfo->fptr_src_image); // reading the 8 bytes from beautiful.bmp (after skpping the 54 bytes)

        if (encode_byte_to_lsb(data[i], encInfo->image_data) == e_success) // calling the byte_to_lsb function and passing the character by character of magic string
        {

            fwrite(encInfo->image_data, 1, 8, encInfo->fptr_stego_image); //writing the encoded data into setgo.bmp
        }
    }

    return e_success;
}

// encoding the characters into stego.bmp
Status encode_byte_to_lsb(char data, char *image_data)
{
    int count = 0;
    for (int i = 0; i < 8; i++)
    {
        count++;
        image_data[i] = (image_data[i] & 0xFE) | ((data & (1 << i)) >> i);
    }

    if (count == 8)
    {
        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// encoding the secret file extention size
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{

    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.')); // copying the .txt into a variable

    if (encode_size_to_lsb(strlen(encInfo->extn_secret_file), encInfo) == e_success) // calling the size to lsb function and passing the .txt size which is 32 and structure
    {

        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// encoding the integer value into stego.bmp
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{

    int buffer;
    int count = 0;

    for (int i = 0; i < 32; i++) // its integer so 32 bits are needed for encoding a integer
    {
        count++;
        fread(&buffer, 1, 1, encInfo->fptr_src_image);

        buffer = (buffer & ~1) | ((size & 1 << i) >> i);

        fwrite(&buffer, 1, 1, encInfo->fptr_stego_image);
    }

    if (count == 32) // if 32 bits are encoded then return e_success or return e_failure
    {

        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// encoding the secret file extention
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{

    int size = strlen(file_extn); // storing the size of .txt
    if (encode_data_to_image(file_extn, size, encInfo) == e_success) //calling the data to image function for encoding the .txt
    {

        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// encoding the secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{

    if (encode_size_to_lsb(file_size, encInfo) == e_success) // calling size to lsb to encoding the integer value
    {
        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// encoding the secret data
Status encode_secret_file_data(EncodeInfo *encInfo)
{

    rewind(encInfo->fptr_secret); // rewindig the pointer to its starting positon

    fread(encInfo->secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret); // reading the data 

    if (encode_data_to_image(encInfo->secret_data, encInfo->size_secret_file, encInfo) == e_success) // encoding the data 
    {

        return e_success;
    }
    else
    {

        return e_failure;
    }
}

// copying the remaing data into stego.bmp without encoding
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1];

    while (fread(buffer, 1, 1, fptr_src))
    {

        fwrite(buffer, 1, 1, fptr_dest);
    }

    return e_success;
}

// calling this function which conten all the encoding functions
Status do_encoding(EncodeInfo *encInfo)
{

    if (open_files(encInfo) == e_success)
    {
        // read the magic string in here
        printf("Enter the magic string : ");
        scanf(" %s", encInfo->magic_string);
        printf("\n");
    }
    if (check_capacity(encInfo) == e_success)
    {

        if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        {

            if (encode_magic_string(encInfo->magic_string, encInfo) == e_success)
            {
                printf("Successfully encoded the magic string\n\n");
                sleep(1);
            }
            else
            {
                printf("Error in magic encoding\n");

                return e_failure;
            }
            if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
            {
                printf("Successfully encoded the secret size of extention\n\n");
                sleep(1);
            }
            else
            {

                printf("Error in secret size extention encoding\n\n");
                return e_failure;
            }

            if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
            {
                printf("Successfully encoded the secret file extention\n\n");
                sleep(1);
            }
            else
            {

                printf("Error in sercret file extention encoding\n\n");
                return e_failure;
            }

            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
            {
                printf("Successfully encoded the size of secret file\n\n");
                sleep(1);
            }
            else
            {

                printf("Error in sercret file size encoding\n\n");
                return e_failure;
            }

            if (encode_secret_file_data(encInfo) == e_success)
            {
                printf("Successfully encoded the secret data\n\n");
                sleep(1);
            }
            else
            {
                printf("Error in encode secret file data\n\n");

                return e_failure;
            }

            if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {

                printf("Successfully encoded remaning img data\n\n");
                sleep(1);
            }
            else
            {

                printf("Error in copy remaing img data\n\n");
                return e_failure;
            }

            return e_success;
        }
    }
}
