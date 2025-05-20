#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h" 

int main(int argc, char *argv[])
{
    EncodeInfo encInfo; // Structure variable for encoding
    DecodeInfo decInfo; // Structure variable for decoding
    if(argc == 1) // Check if command-line arguments are provided
    {
	printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
    }
    else
    {
	int ret = check_operation_type(argv); // Determine the operation type (encode or decode)
	if(ret == e_encode) // Perform actions based on the operation type
	{
	    if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success) // Validate and read encoding arguments
	    {
		if( do_encoding(&encInfo) == e_success )
		{
		    printf("INFO: ## Encoding Done Successfully ##\n");	 
		}
		else
		{
		    printf("INFO: ## Encoding Failed ##\n");	 
		}
	    }
	    else
	    {
		// Validation or reading of encoding arguments failed
		printf("INFO: Read and validate failed\n");
		printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
	    }
	}
 
	if(ret == e_decode)
	{
	    if(read_and_validate_decode_args(argc, argv, &decInfo) == e_success) // Validate and read decoding arguments
	    {
		if( do_decoding(argv,&decInfo) == e_success)
		{
		    printf("INFO: ## Decoding Done Successfully ##\n");	 
		}
		else
		{
		    printf("INFO: ## Decoding Failed ##\n");	 
		}
	    }
	    else
	    {
		// Validation or reading of decoding arguments failed
		printf("INFO: Read and validate failed\n");
		printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
	    }
	}
    }
}

OperationType check_operation_type(char *argv[]) // Function to check the operation type based on command-line arguments
{
    if(strcmp(argv[1],"-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}

