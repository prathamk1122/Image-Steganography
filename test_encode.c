#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include "decode.h"

// making structure variables
EncodeInfo encInfo;
DecodeInfo decode;

int main(int argc, char *argv[])
{

    // checking if argument count is less than or equal to 2 or not
    if (argc <= 2)
    {

        printf("Insufficient argument\n");

        printf("\nFormat should be :- \n");
        printf("\t./a.out -e source.bmp secret_file [destination.bmp]\n ");
        printf("\t./a.out -d destination.bmp [output]\n");
        return e_failure;
    }

    // checking if argument count is greater than 2 or not
    if (argc > 2)
    {
        // checking the which operation user want to perform decoding or encoding
        if (check_operation_type(argv, &argc) == e_encode)
        {
            // rading and checking the cla arguments
            if (read_and_validate_encode_args(argv, &encInfo, argc) == e_success)
            {
                // doing the encoding in this function
                if (do_encoding(&encInfo) == e_success)
                {

                    printf("Encoded the message successfully\n");

                    fclose(encInfo.fptr_src_image);
                    fclose(encInfo.fptr_secret);
                    fclose(encInfo.fptr_stego_image);
                }
                else
                {

                    printf("Error in encoding the message\n");
                }
            }
            else
            {

                printf("Insufficient argument\n");

                printf("\nFormat should be :- \n");
                printf("\t./a.out -e source.bmp secret_file [destination.bmp]\n ");
                printf("\t./a.out -d destination.bmp [output]\n");
                return e_failure;
            }
        }
        
        else if (check_operation_type(argv, &argc) == e_decode)
        {

            if (read_and_check_argument(argv, &decode) == d_success)
            {
                // doing the decoding in this function
                do_decoding(argv, &encInfo, &decode);

                    fclose(decode.fptr_stego_image);
                 
            }
        }
        else if (check_operation_type(argv, &argc) == e_unsupported)
        {
            printf("invalid\n");
        }
    }

    return 0;
}
