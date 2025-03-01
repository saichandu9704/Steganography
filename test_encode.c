#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;   
    if(argc<2)
    {
        printf("Encoding : ./a.out -e <.bmp file> <.txt file> [outputfile]\n");
        printf("Decoding : ./a.out -d <.bmp file> [outputfile]\n");
        return 1;
    }
    printf("INFO: Checking Operation Type..\n");
    if(check_operation_type(argv)==e_encode)
    {
        printf("SUCCESS: Check Operation Function Completed!\n");
        printf("-----Your Choosen Option is  -----\n");
        if(read_and_validate_encode_args(argc,argv,&encInfo)==e_success)
        {
            printf("INFO: Validated Succesfully\n");
            if(do_encoding(&encInfo)==e_success)
            {
                printf("\n\n## Encoded successfully!! ##\n\n");
            } 
            else
            {
                printf("ERROR: Encodeing is Failed\n");
            }
        }
        else
        {
            printf("ERROR: Encode Validation Failed\n");
        }
    }
    else if(check_operation_type(argv)==e_decode)
    {
        printf("SUCCESS: Check Operation Function Completed!\n");
        printf("\n-----Your Choosen Option is DECODE-----\n\n"); 
        if(read_and_validate_decode_args(argc,argv,&decInfo)==d_success)
        {
            printf("INFO: Validated Succesfully\n");
            if(do_decoding(&decInfo)==d_success)
            {
                printf("\n\n## Decoding Done Successfully!! ##\n\n");
            }
            else
            {
                printf("ERROR: Decoding is Failed\n");
            }
        }
        else
        {
            printf("ERROR: Decode Validation Failed\n");
        }
    }
    else
    {
        printf("SUCCESS: Check Operation Function Completed!\n");
        printf("ERROR: \\ (-e,-d ) Please Choose Correct Operation\n");
    }
    return 0;
}
