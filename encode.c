#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file \"%s\"\n", encInfo->src_image_fname);
        return e_failure;
    }
    fprintf(stderr, "INFO: Opened \"%s\"\n", encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file \"%s\"\n", encInfo->secret_fname);

        return e_failure;
    }
    fprintf(stderr, "INFO: Opened \"%s\"\n", encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file \"%s\"\n", encInfo->stego_image_fname);

        return e_failure;
    }
    fprintf(stderr, "INFO: Opened \"%s\"\n", encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}


//checking for encoding or decoding
OperationType check_operation_type(char *argv[])
{
    //encoding
    if (!strcmp(argv[1], "-e"))
        return e_encode;
    //decoding
    else if (!strcmp(argv[1], "-d"))
        return e_decode;
    else
        return e_unsupported;
}


//validating all the files and storing 
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{
    if((argc==4) || (argc==5))
    {
        if (strstr(argv[2], ".bmp"))
        {
            encInfo->src_image_fname = argv[2];

            if (strstr(argv[3], ".txt")||strstr(argv[3], ".c")||strstr(argv[3], ".sh"))
            {
                encInfo->secret_fname = argv[3];
                //printf("%s\n",strchr(encInfo->secret_fname, '.'));

                if (argv[4])
                {
                    printf("INFO: File Name Given By User\n");
                    if (strstr(argv[4], ".bmp"))
                    {
                        encInfo->stego_image_fname = argv[4];
                    }
                    else
                    {
                        printf("Encoding : ./a.out -e <.bmp file> <.txt file> [outputfile]\n");
                        return e_failure;
                    }  
                }
                else
                {
                    printf("INFO: File Taken Defaultly %s\n","24028A.bmp");
                    encInfo->stego_image_fname = "24028A.bmp";
                }
            }
            else
            {
                printf("Encoding : ./a.out -e <.bmp file> <.txt file> [outputfile]\n");
                return e_failure;
            }
            return e_success;
        }
        else
        {
            printf("Encoding : ./a.out -e <.bmp file> <.txt file> [outputfile]\n");
            return e_failure;
        }
    }
    else
    {
        printf("Encoding : ./a.out -e <.bmp file> <.txt file> [outputfile]\n");
        return e_failure;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opeaning Required Files...\n");
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Done\n");
        get_image_size_for_bmp(encInfo->fptr_src_image);
    }
    else
    {
        return e_failure;
    }

    printf("\n## Encoding Procedure Started ##\n\n");

    char magic_string[10];
	printf("Enter the magic string:\n");
	scanf(" %[^\n]", magic_string);
    if(strlen(magic_string)>6)
    {
        printf("ERROR: Magic String Should Be Lessthan 6 Characters\n");
        return e_failure;
    }

    printf("INFO: Checking Capicity...\n");
    if (check_capacity(encInfo) == e_success)
    {
        printf("INFO: Checking Capicity Done!\n");
        printf("SUCCESS: File \"%s\" can Handle Capacity of \"%s\"\n",encInfo->secret_fname,encInfo->stego_image_fname);
    }
    else
    {
        printf("ERROR: File \"%s\" can't Handle Capacity of \"%s\"\n",encInfo->secret_fname,encInfo->stego_image_fname);
        return e_failure;
    }

    printf("INFO: Copying Header (54 byte)\n");
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("SUCCESS: Copied Header File DONE\n");
    }
    else
    {
        printf("ERROR: Copying Header Failed\n");
        return e_failure;
    }
    
    printf("INFO: Encoding Magic String...\n");
    if (encode_magic_string(magic_string, encInfo) == e_success)
    {
        printf("SUCCESS: Encoded Magic String Successfully!\n");
    }
    else
    {
        printf("ERROR: Encode Magic String Is Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding \"%s\" File Extension Size...\n",encInfo->secret_fname);
    if(encode_extn_file_size(strlen(strchr(encInfo->secret_fname,'.')),encInfo)==e_success)
    {
        printf("SUCCESS: Encoded \"%s\" Extention Size\n",encInfo->secret_fname);
    }
    else
    {
        printf("ERROR: Extention Size Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding \"%s\" File Extension...\n",encInfo->secret_fname);
    if (encode_secret_file_extn(strchr(encInfo->secret_fname, '.'), encInfo) == e_success)
    {
        printf("SUCCESS: Encoded \"%s\" File Extention\n",encInfo->secret_fname);
    }
    else
    {
        printf("ERROR: Extension Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding \"%s\" File Size...\n",encInfo->secret_fname);
    if (encode_secret_file_size(get_file_size(encInfo->fptr_secret), encInfo) == e_success)
    {
        printf("SUCCESS: Encoded \"%s\" File Size\n",encInfo->secret_fname);
    }
    else
    {
        printf("ERROR: File Size Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding \"%s\" File Data...\n",encInfo->secret_fname);
    if(encode_secret_file_data(encInfo)== e_success)
    {
        printf("SUCCESS: Encoded \"%s\" File Data\n",encInfo->secret_fname);
    }
    else
    {
        printf("ERROR: File Data Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Copying Remaing Data...\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("SUCESS: Copied Remaing Data Sucessfully\n");
        return e_success;
    }
    else
    {
        printf("ERROR: Remaing Data Copying Failed\n");
        return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    int size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for \"%s\" File Capicity to Handle \"%s\"\n",encInfo->secret_fname,encInfo->stego_image_fname);
    if (get_file_size(encInfo->fptr_src_image) > (54 + strlen(MAGIC_STRING) + 4 + strlen(strchr(encInfo->secret_fname, '.')) + 4 + get_file_size(encInfo->fptr_secret) * 8))
    {
        return e_success;
    }
    else
        return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);
    if (ftell(fptr_dest_image) == 54)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for (int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], arr);
        fwrite(arr, 8, 1, fptr_stego_image);
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & 0xfe | data >> i & 0x01;
        //image_buffer[i] = 0xFF;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    if (encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image((char *)file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    if(encode_size_to_lsb(file_size,encInfo)==e_success)
    {
        return e_success;
    }
  
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int size=get_file_size(encInfo->fptr_secret);
    char buff[8];
    rewind(encInfo->fptr_secret);
    for (int i = 0; i < size; i++)
    {
        fread(buff,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(fgetc(encInfo->fptr_secret),buff);
        fwrite(buff,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src))
    {
      fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

Status encode_extn_file_size(long size,EncodeInfo *encInfo)
{
    if(encode_size_to_lsb(size,encInfo)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_size_to_lsb(long size,EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr,32,1,encInfo->fptr_src_image);
    for (int i = 0; i < 32; i++)
    {
        arr[i] = arr[i] & 0xFFFFFFFE | (size >> i) & 0x00000001;
    }
    fwrite(arr,32,1,encInfo->fptr_stego_image);
    return e_success;
}