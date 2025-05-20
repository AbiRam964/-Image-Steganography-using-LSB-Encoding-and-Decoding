#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_decode_args(int argc, char *argv[],DecodeInfo *decInfo) // Function to read and validate command-line arguments for decoding
{
    if(argc >= 3 && argc <= 4) // Check if the correct number of arguments (3 or 4) is provided
    {
	if( (strstr(argv[2],".") != NULL) && (strcmp(strstr(argv[2],"."),".bmp") == 0) )  //check argv[2] is .bmp file or not
	{
	    decInfo -> src_image_fname = argv[2]; // Set the source image file name
	    if(argv[3] != NULL) // Check if the optional output file name is provided
	    {
		strcpy(decInfo -> secret_fname,argv[3]); 
	    }
	    else
	    {
		strcpy(decInfo -> secret_fname,"decoded"); // Set default output file name if not provided
	    }
	}
	else
	{
	    return e_failure; // Return failure if the second argument does not have a ".bmp" extension
	}
    }
    else
    {
	return e_failure; // Return failure if the number of arguments is not within the expected range
    }
    return e_success;
}
Status do_decoding(char *argv[],DecodeInfo *decInfo) // Function to perform decoding steps 
{
    printf("INFO: ## Decoding Procedure Started ##\n"); 
    //Function calls
    printf("INFO: Opening required files\n");
    if(open_files_decode(decInfo) == e_success) // Open required files for decoding
    {
	printf("INFO: Decoding Magic String Size\n");
	if(decode_magic_string_size(decInfo) == e_success) // Decode magic string size
	{
	    printf("INFO: Done\n");
	    printf("INFO: Decoding Magic String Signature\n");
	    if(decode_magic_string(MAGIC_STRING,decInfo) == e_success) // Decode magic string
	    {
		printf("INFO: Done\n");
		printf("INFO: Decoding Output File Extension Size\n");
		if(decode_secret_file_extn_size(decInfo) == e_success) // Decode output file extension size
		{
		    printf("INFO: Done\n");
		    printf("INFO: Decoding Output File Extension \n");
		    if(decode_secret_file_extn(argv,decInfo) == e_success) // Decode output file extension
		    {
			printf("INFO: Done\n");
			if(open_secret_fname(argv,decInfo) == e_success) // Open the secret file for writing
			{
			    printf("INFO: Opened %s\n",decInfo -> secret_fname);
			    printf("INFO: Done. Opened all required files\n");
			    if(decode_secret_file_size(decInfo) == e_success) // Decode secret file size
			    {
				printf("INFO: Decoding %s File Size\n",decInfo -> secret_fname);
				printf("INFO: Done\n");
				if(decode_secret_file_data(decInfo) == e_success) // Decode secret file data
				{
				    printf("INFO: Decoding %s File Data\n",decInfo -> secret_fname);
				    printf("INFO: Done\n");
				}
				else
				{
				    printf("decode_secret_file_data is fail\n"); // Print error message and return failure
				    return e_failure;
				}
			    }
			    else
			    {
				printf("decode_secret_file_size failed\n"); // Print error message and return failure
				return e_failure;
			    }
			}
			else
			{
			    printf("open_secret_fname failed\n"); // Print error message and return failure
			    return e_failure;
			}
		    }
		    else
		    {
			printf("decode_secret_file_extn failed\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("decode_secret_file_extn_size failed\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("decode_magic_string failed\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("decode_magic_string_size failed\n");
	    return e_failure;
	}		      
    }
    else
    {
	printf("File failed to open\n");
	return e_failure;
    }
    return e_success;
} 

char decode_byte_to_lsb(char *image_buffer) // Function to decode a byte to LSB 
{
    char ch = 0;
    for(int i = 7;i >= 0;i--) // Loop through each bit in the byte and extract LSB
    {
	ch = ch | (image_buffer[7 - i] & 1) << i;
    }
    return ch;
}

int decode_size_to_lsb(char *image_buffer) // Function to decode a 32-bit size value from LSB-encoded bytes 
{
    int size = 0;
    for(int i = 31;i >= 0;i--)  // Loop through each bit in the 32-bit size value and extract LSB
    {
	size = size | ((image_buffer[31 - i] & 1) << i);  
    }
    return size;
}

Status decode_magic_string_size(DecodeInfo *decInfo) // Function to decode magic string size from the source image 
{
    fseek(decInfo -> fptr_src_image,54,SEEK_SET); // Move file pointer to the position where magic string size is stored
    char str[32];
    //fread -> read 32 bytes -> encInfo -> fptr_src_image
    fread(str,1,32,decInfo -> fptr_src_image);
    decInfo -> magic_string_size = decode_size_to_lsb(str);
    //printf("%d\n",decInfo -> magic_string_size);
    return e_success;
}

Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo) // Function to decode magic string from the source image 
{
    int i;
    char str[8];
    char check_magic[(decInfo -> magic_string_size) + 1];
    //run loop -> strlen(magic_string)
    for(i = 0;i < decInfo -> magic_string_size;i++) // Loop through each character in the magic string
    {
	//fread -> 8bytes -> encInfo -> fptr_src_image
	fread(str,8,1,decInfo -> fptr_src_image); // Read 8 bytes from the source image and decode each byte
	check_magic[i] = decode_byte_to_lsb(str);
    } 
    check_magic[i] = '\0';
    if(strcmp(check_magic,magic_string) == 0) // Compare the decoded magic string with the expected magic string
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo) // Function to decode secret file extension size from the source image 
{
    char str[32];
    //fread -> 32 bytes -> encInfo ->fptr_src_image
    fread(str,32,1,decInfo -> fptr_src_image); // Read 32 bytes from the source image and decode size value
    decInfo -> size_secret_file_extn = decode_size_to_lsb(str);
    return e_success;
}

Status decode_secret_file_extn(char *argv[],DecodeInfo *decInfo) // Function to decode secret file extension from the source image 
{
    int i;
    char str[8];
    char check_extn[decInfo -> size_secret_file_extn + 1];
    //run the loop -> strlen(file_extn)
    for(i = 0;i < decInfo -> size_secret_file_extn;i++) // Loop through each character in the file extension
    {
	//fread -> 8bytes -> encInfo -> fptr_src_image
	fread(str,8,1,decInfo -> fptr_src_image); // Read 8 bytes from the source image and decode each byte
	check_extn[i] = decode_byte_to_lsb(str);
    }
    check_extn[i] = '\0';
    //printf("1%s\n",decInfo -> secret_fname);
    //printf("1%s\n",check_extn);
    // Concatenate the decoded extension with the secret file name
    if( strcmp(check_extn,".c") == 0 || strcmp(check_extn,".txt") == 0 || strcmp(check_extn,".sh") == 0)
    {
	if(strstr(decInfo -> secret_fname,".") != NULL)
	{
	    if(strcmp(strstr(decInfo -> secret_fname,"."),".txt") != 0)
	    {
		strcat(decInfo -> secret_fname,check_extn);
	    }
	}
	else
	{
	    strcat(decInfo -> secret_fname,check_extn);
	}
	//printf("2%s\n",decInfo -> secret_fname);
    }
    else
    {
	return e_failure;
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo) // Function to decode secret file size from the source image 
{
    char str[32];

    //fread -> 32bytes -> encInfo -> fptr_src_image
    fread(str,32,1,decInfo -> fptr_src_image); // Read 32 bytes from the source image and decode size value
    decInfo -> size_secret_file = decode_size_to_lsb(str);
    //printf("%d\n",decInfo -> size_secret_file);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo) // Function to decode secret file data from the source image and write to the secret file 
{
    // Array to store secret file data
    char secret_data[decInfo -> size_secret_file];
    //move the file pointer to 0th pointer -> fptr_secret

    char str[8];
    //run the loop -> size_secret_file times
    for(int i = 0;i < decInfo -> size_secret_file;i++) // Loop through each byte in the secret file data
    {
	fread(str,8,1,decInfo -> fptr_src_image); // Read 8 bytes from the source image and decode each byte
	secret_data[i] = decode_byte_to_lsb(str);
    }
    //printf("%s",secret_data);
    fwrite(secret_data,decInfo -> size_secret_file,1,decInfo -> fptr_secret); // Write the decoded data to the secret file
    return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r"); // Open the source image file for reading
    // Do Error handling
    if (decInfo -> fptr_src_image == NULL) // Handle errors if the file fails to open
    {
	perror("fopen");
	fprintf(stderr, "EvRROR: Unable to open file %s\n", decInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n",decInfo -> src_image_fname);
    return e_success;
}

Status open_secret_fname(char *argv[],DecodeInfo *decInfo)
{
    // Secret file
    decInfo -> fptr_secret = fopen(decInfo->secret_fname, "w"); // Open the secret file for writing
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
	return e_failure;
    }
    return e_success;
}
