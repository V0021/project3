#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if(check_operation_type(argv) == e_encode)
    {
        if (argc < 4)
        {
            printf("ERROR: Missing args\n");
            return e_failure;
        }

        printf("You have choosen encoding\n");
        EncodeInfo encInfo;
        
        if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            printf("Every file is not present\n");
            return e_failure;
        }

        if(do_encoding(&encInfo) == e_success)
        {
            printf("\nEncoding is successfully done....\n");
        }
        else
        {
            printf("Encoding is not successfully done.....\n");
        }

        fclose(encInfo.fptr_src_image);
        fclose(encInfo.fptr_secret);
        fclose(encInfo.fptr_stego_image);

    }
    else if (check_operation_type(argv) == e_decode)
    {
        if (argc < 3)
        {
            printf("ERROR: Missing args\n");
            return d_failure;
        }

        printf("You have choosen decoding\n");
        DecodeInfo decInfo;
        
        if(read_and_validate_decode_args(argv, &decInfo) == d_failure)
        {
            printf("Every file is not present\n");
            return d_failure;
        }

        if(do_decoding(&decInfo) == d_success)
        {
            printf("\nDecoding is successfully done....\n");
        }
        else
        {
            printf("Decoding is not successfully done.....\n");
        }
    }
    else
    {
        printf("ERROR: Unsupported option\n");
        return e_failure;
    }
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