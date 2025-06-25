#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h" 
#include "common.h"

/* Functions Definitions */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //Validate .bmp file
    if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if(argv[3] != NULL)
    {
        decInfo -> output_fname = argv[3];
    }
    else
    {
        decInfo -> output_fname = "output.txt";
    }
    return e_success;
}
Status open_decode_files(DecodeInfo *decInfo)
{
    /*open stego.bmp in read mode*/
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    /*if file is null throw error*/
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    /*open decode.txt file in write mode*/
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    /*error handling*/
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

    	return e_failure;
    }
    return e_success;
    
}
char decode_byte_from_lsb(char *image_buffer)
{
    char byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte = (byte << 1) | (image_buffer[i] & 1);
    }

    return byte;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3]; /* it contains 2 charater and '\0' */
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    for(int i = 0; i < 2; i++)
    {
        fread(decInfo->extn_secret_file, 8, sizeof(char), decInfo->fptr_stego_image);
        magic_string[i] = decode_byte_from_lsb(decInfo->extn_secret_file);
    } 
    magic_string[2] = '\0';

 /*
    to debug
    printf("Decoded Magic String: %s\n", magic_string);
 */   

    /* check the encoded magic string and decoded magic string are equal*/
    if (strcmp(magic_string, MAGIC_STRING) != 0)
    {
        fprintf(stderr, "ERROR: Magic string does not match\n");
        return e_failure;
    }
    return e_success;
}
Status decode_size_from_lsb(char *image_buffer, int *size)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        *size = (*size << 1) | (image_buffer[i] & 1);
    }
    return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &decInfo->size_secret_file);
    printf("Decoded size of Secret File Extension : %d bytes\n", decInfo->size_secret_file);
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    for(int i = 0; i < 4; i++)
    {
        fread(buffer, 8, sizeof(char), decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(buffer);      
    }
    /* Add NULL character at end */
    decInfo->extn_secret_file[4] = '\0';
    /* print the extension */
    printf("Decoded Secret File extension : %s\n",decInfo->extn_secret_file);
    return e_success;
}
int decoded_size = 0;
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &decoded_size);
    decInfo->size_secret_file = decoded_size;
    printf("Decoded File Size: %d bytes\n", decoded_size);
    return e_success;
} 
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decoded_size; i++)
    {
        fread(decInfo->extn_secret_file, 8, sizeof(char), decInfo->fptr_stego_image);
        ch = decode_byte_from_lsb(decInfo->extn_secret_file);
        fwrite(&ch, sizeof(char), 1, decInfo->fptr_output);
    }
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
        printf("Opened all the required Decode files Successfully\n");
        if(decode_magic_string(decInfo) == e_success)
        {
            printf("MAGIC STRING is Decoded Successfully\n");
            if(decode_secret_file_extn_size(decInfo) == e_success)
            {
                printf("Decoded Secret File Extension Size Successfully\n");
                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    printf("Decoded Secret File Extension Successfully\n");
                    if(decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("Decoded Secret File Size Successfully\n");
                        if(decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("Decoded Secret File Data Successfully\n");
                        }
                        else
                        {
                            printf("Failed to Decode the Secret File Data\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to Decode the Secret File Size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to Decode the Secret File Extension\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to Decode the Secret File Extension Size\n");
                return e_failure;
            }
        }
        else
        {
            printf("Failed to Decode MAGIC STRING\n");
            return e_failure;
        }
    }
    else
    {
        printf("Failed to open the required Decode files\n");
        return e_failure;
    }
    return e_success;
}
