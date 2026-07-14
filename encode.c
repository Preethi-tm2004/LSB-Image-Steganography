/*including all the required headers*/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)  //function definition for validating arguments for encoding
{
    if(argv[2] == NULL || argv[3] == NULL)
    {
        printf("Error: Insufficient arguments for encoding\n");
        return e_failure;
    }
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp file present\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("Error: .bmp file is not present\n");
        return e_failure;
    }

    if(strstr(argv[3], ".txt") != NULL)
    {
        printf(".txt file present\n");
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf("Error: .txt file is not present\n");
        return e_failure;
    }

    if(argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
        printf("Stegged file is present\n");
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo) //function definition for encoding operation complete
{
    printf("\n\033[34m<----------Started doing encoding---------->\033[0m\n");
    
    if(open_files(encInfo) == e_success) //cheking whether all the required files opened or not
    {
        printf("All files opened successfully\n");
    }
    else
    {
        printf("Error: Failed to open all the files\n");
        return e_failure;
    }

    printf("Enter magic string : "); //reading the magic string from the user 
    scanf(" %[^\n]", encInfo->magic);

    if(check_capacity(encInfo) == e_success) //checking for capacity of source file
    {
        printf("Capacity check successfully\n");
    }
    else
    {
        printf("Error: Capacity of source file is less\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) //checking whether the bmp header is copied or not
    {
        printf("Header copied successfully\n");
    }
    else
    {
        printf("Error: Failed to copy bmp Header\n");
        return e_failure;
    }

    if(encode_magic_string(encInfo->magic, encInfo) == e_success) //cheking for encoding of the magic string
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Error: Failed to encode Magic string\n");
        return e_failure;
    }

    char *extn = strchr(encInfo->secret_fname, '.'); //storing the address of the secret file from the extension to find the length of the extension

    if(extn != NULL)
    {
        int extn_size = strlen(extn); //storing the extension length
        if(encode_secret_file_extn_size(extn_size, encInfo) != e_success) //checking whether secret file extension size encoded successfully or not
        {
            printf("Error: Failed to encode Extension size\n");
            return e_failure;
        }
        if(encode_secret_file_extn(extn, encInfo) == e_success) //cheking whether secret file extension encoded or not
        {
            printf("Secret file extension encoded successfully\n");
        }
        else
        {
            printf("Error: Failed to encode secret file extension\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error: No extension found in secret file\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success) //encoding secret file size
    {
        printf("Secret file size encoded successfully\n");
    }
    else
    {
        printf("Error: Failede to encode Secret file size\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_success) //encoding the data from secret file
    {
        printf("Secret file data encoded successfully\n");
    }
    else
    {
        printf("Error: Failed to encode Secret file data\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) //copying the left over data from the image
    {
        printf("Remaining image data copied successfully\n");
    }
    else
    {
        printf("Error: Failed to encode Remaining image data\n");
        return e_failure;
    }

    /*closing all the files opened*/
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}

Status open_files(EncodeInfo *encInfo) //function definition for opening all the files required
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb"); //opening the source image in read mode

    if(encInfo->fptr_src_image == NULL)
    {
        printf("Error: Failed to open Source file\n");
        return e_failure;
    }
    else
    {
        printf("Source file is present\n");
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb"); //opening the secret file in read mode

    if(encInfo->fptr_secret == NULL)
    {
        printf("Error: Failed to open Secret file\n");
        return e_failure;
    }
    else
    {
        printf("Secret file is present\n");
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb"); //opening the stegged image in write mode

    if(encInfo->fptr_stego_image == NULL)
    {
        printf("Failed to open %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    printf("Stego file is opened\n");

    return e_success;
    
}

Status check_capacity(EncodeInfo *encInfo) //function definition to check image capacity
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); //storing image size inside image capacity
    //printf("Image capacity = %u\n", encInfo->image_capacity);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); //storing file size inside secret file size
    //printf("Size of secret file = %lu\n", encInfo->size_secret_file);

    /*cheking whether image capacity is more than all the source, secret and magic informations memory stored*/
    if(encInfo->image_capacity > (strlen(encInfo->magic) *8 + 32 + 32 + 32 + encInfo->size_secret_file * 8)) 
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

    return e_success;
    
}

uint get_image_size_for_bmp(FILE *fptr_image) //function definition to find image size for bmp
{
    int wid , len; 

    fseek(fptr_image, 18, SEEK_SET); //moving the source file pointer to the 18th position to get the width of the bmp*/

    /*reading width and length from bmp*/
    fread(&wid, 4, 1, fptr_image);

    fread(&len, 4, 1,fptr_image);

    rewind(fptr_image); //rewinding the pointer back to initial position

    return wid * len * 3; //returning the image size for bmp 
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0 ,SEEK_END); //moving secret file pointer to the end to get file size
    long size = ftell(fptr); //finding the size using ftell
    rewind(fptr); //again rewinding the file pointer to initial
    return size; //returning file size
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) //function definition to copy bmp header
{
    rewind(fptr_src_image); //rewinding the sorce image file pointer to the start

    char buffer[54]; //declaring array of buffer with size 54

    fread(buffer, 1, 54, fptr_src_image); //reading the first 54 bytes of data
    fwrite(buffer, 1, 54, fptr_dest_image); //copying those 54 bytes of data
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo) //function definition to encode magic string
{
    int magic_len = strlen(magic_string); //storing the length of the magic string inside magic_len

    encode_int_to_lsb(magic_len, encInfo->fptr_src_image, encInfo->fptr_stego_image); //encoding the length of the magic string

    if(encode_data_to_image(magic_string, magic_len, encInfo) == e_success) //checking for encoding the magic string data into the image
    {
        printf("Magic encoded\n");
        return e_success;
    }
    else
    {
        printf("Error: Magic not happened\n");
        return e_failure;
    }
}

Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo) //function definition to encode magic string data to image
{
    char buffer[8]; //declaing the buffer with size 8
    for(int i=0; i < size; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image); //reading the data byte by byte
        encode_byte_to_lsb(data[i], buffer);  //encoding byt to lsb of the buffer
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image); //copying the magic string info to the stegged image
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)  //function definition to encode bytes to lsb of the image
{
    for(int i=7; i >= 0; i--)
    {
        image_buffer[7-i] = ((data & (1 << i)) >> i) | (image_buffer[7-i] & 0XFE);
    }

    return e_success;
}

Status encode_int_to_lsb(int magic_len, FILE *fptr_src, FILE *fptr_dest) //function definition for encoding integer to lsb of the image
{
    char buffer[32];
    fread(buffer, 32, 1, fptr_src); //reading 32 bits from source image 

    for(int i=31; i>=0; i--)
    {
        buffer[31-i] = ((magic_len & (1 << i)) >> i) | (buffer[31-i] & 0xFE);
    }
    fwrite(buffer, 32, 1, fptr_dest); //writing the 32 bits to stegged cimage

    return e_success;
}

Status encode_secret_file_extn_size(int ext_size, EncodeInfo *encInfo) //function definition to encode secret file extension size
{
    if(encode_int_to_lsb(ext_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
        return e_failure;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) //function definition to encode secret file extension
{
    if(encode_data_to_image((char *) file_extn, strlen(file_extn), encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo) //function definition to encode secret file size
{
    if(encode_int_to_lsb((int)file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo) //function definition to encode secret file data inside secret file
{
    char ch;

    while(fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        if(encode_data_to_image(&ch, 1, encInfo) != e_success)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) //function definition to copy left over data from image
{
    char ch;

    while(fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;
}




