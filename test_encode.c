#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h" 
#include "decode.h"

int main(int argc, char **argv)
{
    if(check_operation_type(argv) == e_encode)
    {
        EncodeInfo encInfo;
        printf("**********************Selected Encoding**********************\n");
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and Validate encode Arguments is successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("**********************Completed Encoding**********************\n");
            }
            else
            {
                printf("Failed to Encode the secrete data\n");
            }
        }
        else
        {
            printf("Failed to Validate the arguments\n");
        }
    }
    else if(check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo;
        printf("**********************Selected Decoding**********************\n");
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and Validate Decode Arguments is successful\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("**********************Completed Decoding**********************\n");
            }
            else
            {
                printf("Failed to Decode the secrete data\n");
            }
        }
        else
        {
            printf("Failed to Read and Validate Decode Arguments\n");
        }

    }
    else
    { 
        printf("Invalid Option\nUSAGE\n");
        printf("Encoding : ./a.out -e beautiful.bmp secrete.txt\n");
        printf("Decoding : ./a.out -d stego.bmp\n");
    }
    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}