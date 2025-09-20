#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp is present\n");
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }

    // argv[3] -> accept any file extension (no restriction to .txt)
    char *ext = strrchr(argv[3], '.');
    if (ext != NULL)
    {
        printf("Secret file with extension '%s' is present\n", ext);
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf("ERROR: Secret file does not have an extension\n");
        return e_failure;
    }

    if (argv[4] != NULL)
{
    // Check if the provided stego image name ends with ".bmp"
    if (strstr(argv[4], ".bmp") != NULL)
    {
        printf("Stego image name provided: %s\n", argv[4]);
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        printf("ERROR: Stego image must have a .bmp extension\n");
        return e_failure;
    }
}
else
{
    // No name provided,
    printf("No stego image name provided, provide a name without extension\n");
char name[20];
scanf("%s", name);

strcpy(encInfo->stego_image_fname, name);

strcat(encInfo->stego_image_fname, ".bmp");

}

return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    // Open Source Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        printf("ERROR: Unable to open source image file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Open Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        printf("ERROR: Unable to open secret file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Open Stego Image file for writing
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    // Get total image capacity in bytes (excluding BMP header)
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image) - 54;

    // Calculate secret file size
    long secret_file_size = 0;
    char ch;
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        secret_file_size++;
    }

    // Reset file pointer to beginning of secret file
    rewind(encInfo->fptr_secret);

    // Store secret file size in structure
    encInfo->size_secret_file = secret_file_size;

    // Extract file extension of secret file
    char *r = strrchr(encInfo->secret_fname, '.');
    char ex[10] = "";
    if (r != NULL)
    {
        strcpy(ex, r);
    }

    // Calculate total required size in bits to encode the secret in the image
    // 4 bytes for storing length of magic string
    // strlen(magic_string) bytes for the magic string itself
    // 4 bytes for storing length of file extension
    // strlen(ext) bytes for the extension
    // 4 bytes for storing secret file size
    // secret_file_size bytes for the actual secret file
    long required_size = (4 + strlen(encInfo->magic_string) + 4 + strlen(ex) + 4 + secret_file_size) * 8;

    // Check if image capacity is sufficient
    if (image_capacity >= required_size)
    {
        printf("Image capacity is sufficient.\n");
        return e_success;
    }
    else
    {
        printf("ERROR: Image capacity insufficient\n");
        return e_failure;
    }
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    rewind(fptr_src_image);
    fread(header, 1, 54, fptr_src_image);
    fwrite(header, 1, 54, fptr_dest_image);
    printf("Header is successfully stored\n");
    return e_success;
}

void encode_byte_to_lsb(char data, char *buffer)
{
    int n = 7;

    for (int i = 0; i < 8; i++)
    {
        int mask = 1 << n;
        int bit  = data & mask;
        bit = bit >> n;

        buffer[i] = buffer[i] & 0xFE;
        buffer[i] = buffer[i] | bit;

        n--;
    }

}

void encode_size_to_lsb(long size, char *buffer)
{
    int n = 31;

    for (int i = 0; i < 32; i++)
    {
        int mask = 1 << n;
        int bit  = size & mask;
        bit= bit >> n;
        buffer[i] = buffer[i] & 0xFE;
        buffer[i] = buffer[i] | bit;
        n--;
    }

}

Status encode_magic_string_length(int len, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    printf("Magic string length is stored successfully\n");
    return e_success;
}

Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(magic_str); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_str[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("Magic string is stored successfully\n");
    return e_success;
}

Status get_file_extn_len(EncodeInfo *encInfo)
{
    char *ext = strrchr(encInfo->secret_fname, '.');
    if (ext)
    {
        // Copy extension (including the dot) into struct
        strcpy(encInfo->extn_secret_file, ext);

        encInfo->extn_size = strlen(ext);
        return e_success;
    }
    return e_failure;
}

Status encode_secret_file_extn_len(int extn_len, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(extn_len, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    printf("Secret file extension length is stored successfully\n");
    return e_success;
}

Status encode_secret_file_extn(const char *extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i = 0; i < strlen(extn); i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    printf("Secret file extension is successfully stored\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    printf("Secret file size is successfully stored\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Encode each byte of the secret file
    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        char ch;
        char buffer[8];

        // Read one byte from secret file
        fread(&ch, 1, 1, encInfo->fptr_secret);

        // Read 8 bytes from source image
        fread(buffer, 1, 8, encInfo->fptr_src_image);

        // Encode the secret byte into LSBs of image bytes
        encode_byte_to_lsb(ch, buffer);

        // Write the modified bytes to stego image
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }

    printf("Secret file data successfully stored.\n");
    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    printf("Copy remaining image data is successfully stored\n");
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("File cannot open\n");
        return e_failure;
    }

    printf("Enter a magic string (max %d chars): ", MAX_MAGIC_STRING - 1);
    scanf("%s", encInfo->magic_string);
    printf("Magic string is stored\n");

    if(check_capacity(encInfo) == e_failure)
    {
        printf("Check Capacity is fail\n");
        return e_failure;
    }

    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    encode_magic_string_length(strlen(encInfo->magic_string), encInfo);
    encode_magic_string(encInfo->magic_string, encInfo);
    get_file_extn_len(encInfo);
    encode_secret_file_extn_len(encInfo->extn_size, encInfo);
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    encode_secret_file_data(encInfo);
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}