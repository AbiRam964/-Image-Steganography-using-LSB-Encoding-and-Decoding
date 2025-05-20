#ifndef DECODE_H


#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding output.bmp to decoded.txt
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname; //output.bmp
    FILE *fptr_src_image; //file address of output.bmp file
    int magic_string_size;

    /* Secret File Info */
    FILE *fptr_secret; //File address of secret.txt
    int size_secret_file_extn; //Secret file extension size
    int size_secret_file; //Secret file size
    
    char secret_fname[50]; //secret.txt

} DecodeInfo;

/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/*Decode magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decinfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_to_lsb(char *image_buffer); //decode a char

int decode_size_to_lsb(char *image_buffer); //decode a integer

Status open_secret_fname(char *argv[],DecodeInfo *decInfo);

#endif
