#ifndef DECODE_H
#define DECODE_H

#include "types.h" //contains user defined types

/*sturctures to store information required for
* decoding secret file from stego image
*/

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    char output_fname[100];
    FILE *fptr_output;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;

    int is_output_provided;
    char magic[20];

} DecodeInfo;

/* Decoding function prototypes */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Performs decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open files */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip bmp header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode magic string */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extn_size(int *ext_size, DecodeInfo *decInfo);

/* Decode extension */
Status decode_secret_file_extn(char *extn, int ext_size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Core decoding functions */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *data, char *image_buffer);
Status decode_int_from_lsb(int *data, FILE *fptr_stego);

#endif