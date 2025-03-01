#include <stdio.h>
#include "types.h"
#include "decode.h"
#include "common.h"
#include <string.h>
int count = 0;

Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo)
{
    if((argc==3) || (argc==4))
    {
        if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
        {
            decInfo->encoded_image_fname = argv[2];
            if (argv[3] == NULL)
            {
                printf("INFO: Output File Not_Mentioned, Creating Default Output Image File...\n");
                decInfo->output_fname = "decoded_file";
            }
            else
            {
                printf("INFO: Output File Given By User\n");
                if (strstr(argv[3], ".txt")||strstr(argv[3], ".c")||strstr(argv[3], ".sh"))
                {
                    decInfo->output_fname = argv[3];
                }
                else
                {
                    printf("Decoding : ./a.out -e <.bmp file> [outputfile]\n");
                    return d_failure;
                }
            }
            return d_success;
        }
        else
        {
            printf("Decoding : ./a.out -e <.bmp file> [outputfile]\n");
            return d_failure;
        }
    }
    else
    {
        printf("Decoding : ./a.out -e <.bmp file> [outputfile]\n");
        return d_failure;
    }
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Opening \"%s\" image...\n",decInfo->output_fname);
    if (open_decode_files(decInfo) == d_success)
    {
        printf("SUCCESS: \"%s\" File Opened successfully\n",decInfo->output_fname);
    }
    else
    {
        printf("ERROR: Opening File Failed\n");
        return d_failure;
    }

    printf("\n\n## Decoding Procedure Started ##\n\n");

    printf("INFO: Skipping Header...\n");
    printf("INFO: DONE\n");
    printf("INFO: Decoding Magic String...\n");
	char magic_string[10];
	printf("Enter the magic string(PASSWORD):\n");
	scanf(" %[^\n]", magic_string);

    if (decode_magic_string(magic_string, decInfo) == d_success)
    {
        //printf("decoded magic string successfully\n");
    }
    else
    {
        printf("ERROR: Decoding Magic String Failed\n");
        return d_failure;
    }

    printf("INFO: Decoding Secret File Extension Size...\n");
    if (decode_file_ext_size(decInfo) == d_success)
    {
        printf("SUCCESS: Decoded Secret File Extension Size\n");
    }
    else
    {
        printf("ERROR: Extension Size Decoding Failed\n");
        return d_failure;
    }
    
    printf("INFO: Decoding Secret File Extension...\n");
    if (decode_file_extn(decInfo) == d_success)
    {
        printf("SUCCESS: Decoded Secret File Extension\n");
        char buff[20];
        strcpy(buff, decInfo->output_fname);
        strcat(buff, decInfo->extn_secert_file);
        decInfo->fptr_output = fopen(buff, "w");
        if (decInfo->fptr_output == NULL)
        {
            perror("fopen");
            fprintf(stderr, "Error : Uanble to open file %s\n", decInfo->encoded_image_fname);
            return d_failure;
        }
    }
    else
    {
        printf("ERROR: Extension Decoding Failed\n");
        return d_failure;
    }

    printf("INFO: Decoding Secret File Size...\n");
    if (decode_file_size(decInfo) == d_success)
    {
        printf("SUCCESS: Decoded Secret File Size\n");
    }
    else
    {
        printf("ERROR: Seceret File Size Decoding Failed\n");
        return d_failure;
    }

    printf("INFO: Decoding Secret File Data...\n");
    if (decode_file_data(decInfo) == d_success)
    {
        printf("SUCCESS: Decoded Secret File Data\n");
        return d_success;
    }
    else
    {
        printf("ERROR: Seceret File Data Decoding Failed\n");
        return d_failure;
    }
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_encoded_image = fopen(decInfo->encoded_image_fname, "r");
    // error handling
    if (decInfo->fptr_encoded_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : Uanble to open file %s\n", decInfo->encoded_image_fname);
        return d_failure;
    }
    return d_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char arr[8];
    fseek(decInfo->fptr_encoded_image, 54, SEEK_CUR);
    for (int i = 0; i < strlen(magic_string); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // 1 bit of magic compared with 1 byte
            printf("%c", (magic_string[i] >> j) & 0x01);
            if ((fgetc(decInfo->fptr_encoded_image) & 0x01) != ((magic_string[i] >> j) & 0x01))
            {
                return d_failure;
            }
        }
    }
    printf("INFO: Decoded Magic String\n");
    return d_success;
}

Status decode_file_ext_size(DecodeInfo *decInfo)
{

    if (decode_size_from_lsb(decInfo->fptr_encoded_image) == d_success)
    {
        decInfo->extn_size = count;
        // printf("%ld\n ",decInfo->extn_size);
        return d_success;
    }
    return d_failure;
}

Status decode_file_extn(DecodeInfo *decInfo)
{

    int size = decInfo->extn_size;
    char data[size];
    char arr[8];

    for (int i = 0; i < size; i++)
    {
        data[i] = 0;
        fread(arr, 8, 1, decInfo->fptr_encoded_image);
        for (int j = 7; j >= 0; j--)
        {
            data[i] = data[i] << 1 | arr[j] & 0x01;
        }
        // printf("%c", data[i]);
    }
    data[size] = '\0';
    strcpy(decInfo->extn_secert_file, data);
    return d_success;
}

Status decode_file_size(DecodeInfo *decInfo)
{
    if (decode_size_from_lsb(decInfo->fptr_encoded_image) == d_success)
    {
        decInfo->output_file_size = count;
        return d_success;
    }
    return d_failure;
}

Status decode_size_from_lsb(FILE *fptr_encoded_image)
{
    char arr[32];
    count = 0;
    fread(arr, 32, 1, fptr_encoded_image);
    for (int i = 31; i >= 0; i--)
    {
        count = (count << 1) | arr[i] & 0x01;
    }
    return d_success;
}

Status decode_file_data(DecodeInfo *decInfo)
{
    if (decode_file_from_image(decInfo->output_file_size, decInfo->fptr_encoded_image, decInfo->fptr_output) == d_success)
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

Status decode_file_from_image(int size, FILE *fptr_encoded_image, FILE *fptr_output)
{
    char arr[8];
    char data;
    data = 0;
    for (int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_encoded_image);
        decode_byte_from_lsb(&data, arr);
        fwrite(&data, 1, 1, fptr_output);
    }
    return d_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    for (int j = 7; j >= 0; j--)
    {
        *data = *data << 1 | image_buffer[j] & 0x01;
    }
}