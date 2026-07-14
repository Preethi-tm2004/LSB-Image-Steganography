/*
Name         : Preethi T M
Batch        : 26001B
Date         : 04/05/2026
Project name : LSB image stegnography
Description  : This project implements image steganography using the LSB (Least Significant Bit) technique to securely hide secret data inside an image file.
               It ensures data confidentiality by embedding and extracting information without visibly altering the image.
*/
/*Including all the headers required*/
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char *argv[]) //passing arguments count and arguments to perform validations
{
    if(argc < 2) //cheking for invalid argument
    {
        printf("Error: Invalid arguments\n");
        printf("Usage : \n");
        printf("./a.out -e <bmp file> <txt file> [stego bmp file]\n");
        printf("./a.out -d <bmp file> [stego bmp file]\n");
        return e_failure;
    }

    if(check_operation_type(argv) == e_encode)  //cheking operation type
    {
        printf("You choose encoding\n");
        EncodeInfo encInfo;

        if(read_and_validate_encode_args(argv, &encInfo) == e_success) //checking for validation
        {
            printf("Read and validate successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("\033[32mEncoding successfull\033[0m\n");
                return e_success;
            }
            else
            {
                printf("\033[31mError: Encoding failed\033[0m\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error: Read and validate failed\nInvalid arguments! For encoding try -> ./a.out -e beautiful.bmp secret.txt\n");
            return e_failure;
        }
        return e_success;
    }
    else if(check_operation_type(argv) == e_decode)
    {
        printf("You choose decoding\n");

        DecodeInfo decInfo;

        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validate successful\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("\033[32mDecoding successful\033[0m\n");
                return e_success;
            }
            else
            {
                printf("\033[31mError: Decoding failed\033[0m\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error: Read and validate failed\nInvalid arguments! For decoding try -> ./a.out -d stego.bmp\n");
            return e_failure;
        }

        return e_success;
    }
    
}

OperationType check_operation_type(char *argv[]) //function definition for argument type check
{
    if(argv[1] == NULL)
    {
        return e_unsupported;
    }
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
