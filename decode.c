/*including all the required headers*/
#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) //function definition for validating arguments for decoding
{
    if(argv[2] != NULL)
    {
        printf("Stego file present\n");
    }
    else
    {
        printf("Error: No stego image provided\n");
        return e_failure;
    }

    if(strstr(argv[2], ".bmp") == NULL)
    {
        printf("Error: Invalid file formate! please provide .bmp file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if(argv[3] != NULL)
    {
        strcpy(decInfo->output_fname, argv[3]);
        decInfo->is_output_provided = 1;
        printf("Output file name provided\n");
    }
    else
    {
        decInfo->output_fname[0] = '\0';
        decInfo->is_output_provided = 0;
        printf("Output file name not provided\n");
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo) //function definition for decoding operation complete
{
    printf("\n\033[34m<----------Started doing decoding---------->\033[0m\n");

    if(open_decode_files(decInfo) == e_success) //cheking whether all the required files opened or not
    {
        printf("All the files opened successfully\n");
    }
    else
    {
        printf("Error: Failed to open all the files\n");
        return e_failure;
    }

    if(skip_bmp_header(decInfo->fptr_stego_image) == e_success) //checking whether the bmp header is skipped or not
    {
        printf("BMP header skipped successfully\n");
    }
    else
    {
        printf("Error: Failed to skip bmp header\n");
        return e_failure;
    }

    if(decode_magic_string(decInfo->magic, decInfo) == e_success) //cheking for decoding of the magic string
    {
        printf("Magic string decoded successfully\n");

        char user_magic[20];
        printf("Enter magic string to validate : "); //reading magic string from the user
        scanf(" %[^\n]", user_magic);

        if(strcmp(user_magic, decInfo->magic) != 0) //checking whether the user entered maguc string is matching with the decoded magic string
        {
            printf("Eroor: Magic string mismatch\n");
            return e_failure;
        }
        printf("Magic string matched successfully\n");
    }
    else
    {
        printf("Error: Failed to decode magic string\n");
        return e_failure;
    }

    int extn_size;
    if(decode_secret_file_extn_size(&extn_size,decInfo) != e_success) //checking whether secret file extension size decoded successfully or not
    {
        printf("Error: Failed to decode secret file extension size\n");
        return e_failure;
    }
    if(decode_secret_file_extn(decInfo->extn_secret_file, extn_size, decInfo) == e_success) //cheking whether secret file extension decoded or not
    {
        decInfo->extn_secret_file[extn_size] = '\0';
        printf("Secret file extension decoded successfully\n");
    }
    else
    {
        printf("Error: Failed to decode secret file extension\n");
        return e_failure;
    }

    char final_name[100];

    if(decInfo->is_output_provided == 0) //chicking whether the output file name provided or not
    {
        //default file name
        strcpy(final_name, "decoded");
        strcat(final_name, decInfo->extn_secret_file);

        printf("Output file not mentioned. Creating %s as default\n", final_name);
    }
    else
    {
        char base_name[100];
        char *dot = strchr(decInfo->output_fname, '.');

        if(dot != NULL) //if the name is provided with extension, replacing the . with null
        {
            int len = dot - decInfo->output_fname;
            strncpy(base_name, decInfo->output_fname, len);
            base_name[len] = '\0';
        }
        else
        {
            strcpy(base_name, decInfo->output_fname); //if no extension given copy the base name(just name) to output_fname
        }
        strcpy(final_name, base_name); //copying the base name inside final entered name
        strcat(final_name, decInfo->extn_secret_file); //concatinating secret file extension with final name after decoding the extension
    }

    strcpy(decInfo->output_fname, final_name); //copying final name with extension to output_fname
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb"); //opening output file in write mode

    if(decInfo->fptr_output == NULL)
    {
        printf("Error: Failed to open output file\n");
        return e_failure;
    }
    else
    {
        printf("Opened %s\n", final_name);
        printf("Output file opened successfully\n");
    }

    if(decode_secret_file_size(&decInfo->size_secret_file, decInfo) == e_success) //decoding secret file size
    {
        printf("Secret file size decoded successfully\n");
    }
    else
    {
        printf("Error: Failed to decode secret file size\n");
        return e_failure;
    }

    if(decode_secret_file_data(decInfo) == e_success) //decoding the data from secret file
    {
        printf("Secret file data decoded successfully\n");
    }
    else
    {
        printf("Error: Failed to decode secret file data\n");
        return e_failure;
    }

    /*closing all the files opened*/
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo) //function definition for opening all the files required
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if(decInfo->fptr_stego_image == NULL)
    {
        printf("Error: Failed to open stego file\n");
        return e_failure;
    }

    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego) //function definition to skip bmp header
{

    fseek(fptr_stego, 54, SEEK_SET); //moving the file pointer to 54th position to skip first 54 bytes of data form bmp

    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo) //function definition to decode magic string
{
    int magic_len;

    if(decode_int_from_lsb(&magic_len, decInfo->fptr_stego_image) != e_success)
        return e_failure;

    if(decode_data_from_image(magic_string, magic_len, decInfo) != e_success)
        return e_failure;
    
    magic_string[magic_len] = '\0';

    return e_success;
}

Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo) //function definition to decode magic string data from image
{
    char buffer[8];
    for(int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&data[i], buffer);
    }

    return e_success;
}

Status decode_byte_from_lsb(char *data, char *buffer) //function definition to decode bytes from lsb of the image
{
    *data = 0;
    for(int i = 0; i < 8; i++)
    {
        *data = *data | ((buffer[i] & 1) << (7 - i));
    }

    return e_success;
}

Status decode_secret_file_extn_size(int *ext_size, DecodeInfo *decInfo) //function definition to decode secret file extension size
{
    if(decode_int_from_lsb(ext_size, decInfo->fptr_stego_image) == e_success)
        return e_success;
    else
        return e_failure;
}

Status decode_int_from_lsb(int *data, FILE *fptr)//function definition for decoding integer from lsb of the image
{
    char buffer[32];
    *data = 0;

    fread(buffer, 32, 1, fptr);

    for(int i =0; i < 32; i++)
    {
        *data = *data | ((buffer[i] & 1) << (31 - i));
    }

    return e_success;
}

Status decode_secret_file_extn(char *extn, int extn_size, DecodeInfo *decInfo) //function definition to decode secret file extension
{
    if(decode_data_from_image(extn, extn_size, decInfo) == e_success)
        return e_success;
    else
        return e_failure;
}

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo) //function definition to decode secret file size
{
    int temp;
    if(decode_int_from_lsb(&temp, decInfo->fptr_stego_image) == e_success)
    {
        *file_size = temp;
        return e_success;
    }
    else
        return e_failure;
}

Status decode_secret_file_data(DecodeInfo *decInfo) //function definition to decode secret file data inside secret file
{
    char ch;

    for(long i = 0; i < decInfo->size_secret_file; i++)
    {
        if(decode_data_from_image(&ch, 1, decInfo) != e_success)
        {
            return e_failure;
        }
        fputc(ch, decInfo->fptr_output); //writing secret file data to output file character by character
    }

    return e_success;
}

