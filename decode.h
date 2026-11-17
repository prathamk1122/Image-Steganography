#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include"encode.h"
#include"types.h"
#include"common.h"
#include<string.h>

// structure for strong the values which is needed for decoing the secret message
typedef struct{

    char output_fname[100];
    FILE *fptr_stego_image;
    uint extn_file_size;
    uint secret_file_size;
    char *file_extn;

    char magic_string[100];
    
    


}DecodeInfo;


Status check_magic_string(DecodeInfo *decode);

Status decode_byte_to_lsb(int size,FILE *fptr_to_stego_image,char *buffer);

Status read_and_check_argument(char *argv[],DecodeInfo *decode);

Status decode_size_to_lsb(DecodeInfo *decode,int *dest);

Status decode_secret_file_extn_size(DecodeInfo *decode);

Status decode_secret_file_extn(DecodeInfo *decode );


Status open_file(char *argv[],DecodeInfo *decode);

Status decode_secret_file_size(DecodeInfo *decode);


Status do_decoding(char *argv[],EncodeInfo *encInfo,DecodeInfo *decode);

Status decode_secret_file_data(DecodeInfo *decode);



#endif