#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include <stdio.h>

/*
 * Structure to store information required for
 * decoding secret data from a stego image
 * Includes input/output file info, magic string,
 * secret file extension, and secret data size
 */

#define MAX_MAGIC_STRING 20
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;    // Name of the stego image containing secret data
    FILE *fptr_stego_image;     // File pointer to stego image
    
    /* Output file info */
    char output_fname[100];     // Output file name, extension added after decoding

    /* Magic string to verify encoding */
    char magic_string[MAX_MAGIC_STRING];

    /* Secret file extension */
    char extn_secret_file[MAX_FILE_SUFFIX]; // Extension of decoded secret file
    int extn_size;                          // Length of the secret file extension

    /* Secret file size */
    long size_secret_file;                   // Size of the decoded secret file

} DecodeInfo;


/* Decode-related function prototypes */

/* Read and validate decoding arguments from command line */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the complete decoding process */
Status do_decoding(DecodeInfo *decInfo);

/* Open input/output files required for decoding */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip the BMP header while reading the image */
Status skip_bmp_header(FILE *fptr_src, FILE *fptr_dest);

/* Decode a single byte from LSBs of image data */
void decode_byte_from_lsb(char *data, char *buffer);

/* Decode a size (long) value from LSBs of image data */
void decode_size_from_lsb(long *size, char *buffer);

/* Decode magic string length stored in LSBs */
Status decode_magic_string_len(DecodeInfo *decInfo, int *len);

/* Decode the actual magic string */
Status decode_magic_string(char *magic_str, int len, DecodeInfo *decInfo);

/* Decode secret file extension length */
Status decode_secret_file_extn_len(DecodeInfo *decInfo, int *extn_len);

/* Decode the secret file extension */
Status decode_secret_file_extn(char *extn, DecodeInfo *decInfo);

/* Decode the secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size);

/* Decode the secret file data from the stego image */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
