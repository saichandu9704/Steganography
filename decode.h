#ifndef DECODE_H
#define DECODE_H
#include "common.h"
#include "types.h" 
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

//structure for decode
typedef struct _DecodeInfo 
{
    //Setgo image info 
    char *encoded_image_fname;
    FILE *fptr_encoded_image;


    //secert file info
    char *output_fname;
    FILE *fptr_output;
    char extn_secert_file[4];
    long extn_size;
    long output_file_size;
    
}DecodeInfo;

//Read and validate decode args from argv 
Status read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decInfo);


//Perform the decoding 
Status do_decoding(DecodeInfo *decInfo);


//Get File pointer for i/p and o/p files
Status open_decode_files(DecodeInfo *decInfo);
  
//decodes magic string
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

//decodes file_extn_size 
Status decode_file_ext_size(DecodeInfo *decInfo);

//decodes file_size 
Status decode_file_size(DecodeInfo *decInfo);


//decodes file_extn
Status decode_file_extn(DecodeInfo *decInfo);

//decode file datas
Status decode_file_data(DecodeInfo *decInfo);


//deodes file from image
Status decode_file_from_image(int size,FILE *fptr_encoded_image,FILE *fptr_output);

//decodes byte from lsb
Status decode_byte_from_lsb(char *data,char *image_buffer);

//decodes size from lsb
Status decode_size_from_lsb(FILE *fptr_encoded_image);


#endif