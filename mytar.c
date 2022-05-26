#include <stdio.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>

/* tar Header Block, from POSIX 1003.1-1990.  */
/* POSIX header.  */
struct posix_header
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */
};
char gnu_magic[] = "ustar  ";
char ustar_magic[] = "ustar";

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
size_t BLOCKSIZE = 512;

/* Debug */
void print_header(struct posix_header header)
{
	printf("name[100] = %s", header.name);
	printf("\nmode[8]= %s", header.mode);
	printf("\nuid[8] = %s", header.uid);
	printf("\ngid[8] = %s", header.gid);
	printf("\nsize[12] = %s", header.size);
	printf("\nmtime[12] = %s", header.mtime);
	printf("\nchksum[8] = %s", header.chksum);
	printf("\ntypeflag(char) = %c", header.typeflag);
	printf("\nlinkname[100] = %s", header.linkname);
	printf("\nmagic[6] = '%s'", header.magic);
	printf("\nversion[2] = %s\n", header.version);
}

enum Option 
{
	NO_OPT,
	EXTRACT_OPT,
	LIST_OPT
};

/**
 * @brief Returns cmd-line arguments.
 *
 * Walks the given pointer and returns each string.
 * 
 * @param argv Command-line arguments.
 * @return Argument or character of option or NULL at end of input.
 */
char* get_arg(char*** argv)
{
	while (NULL != **argv)
	{
		if (***argv == '-')
		{
			(*argv)++;
			return **argv;
		}
		else
		{
			(*argv)++;
			return **argv;
		}
	}
	return NULL;
}

/**
 * Describes a tar action
 */
struct tar_action_s
{
	enum Option option;
	char* filename;
	char** file_list;
	int file_list_len;
	int verbose;
};

/**
 * @brief Parses the command-line arguments into a tar_action_s struct.
 * 
 * Walks the argv pointer to parse and check validity of command-line arguments.
 * If succesful then returns a tar_action_s, otherwise exits with an error.
 * 
 * @param argv Command-line arguments.
 * @return Struct describing action of parsed command-line arguments.
 */
struct tar_action_s parse_args(int argc, char** argv)
{
	struct tar_action_s tar_action = {
		.option = NO_OPT,
		.filename = NULL,
		.file_list = calloc(argc, sizeof(char*)),
		.file_list_len = argc,
		.verbose = 0
	};

	if (argc == 1) 
	{
		errx(2, "need at least one option");
	}

	int num_file = 0;
	char* arg;
	while ((arg = get_arg(&argv)) != NULL)
	{
		if (arg[0] == '-')
		{
			switch (arg[1])
			{
				case 't':
					tar_action.option = LIST_OPT;
					break;
				case 'x':
					tar_action.option = EXTRACT_OPT;
					break;
				case 'f':
					/**
					 * If it is specified then the next word in
					 * arguments is the filename.
					 */
					argv++;
					if (NULL == *argv)
					{
						err(64, "option requires an argument -- 'f'");
					}
					tar_action.filename = *argv;
					break;
				case 'v':
					tar_action.verbose = 1;
					break;
				default:
					errx(2, "Unknown option: %s", arg);
					break;
			}
		}
		else
		{
			tar_action.file_list[num_file] = *argv;
			num_file++;
		}
	}

	if (NULL != tar_action.filename && tar_action.option == NO_OPT) 
	{
		errx(2, "You must specify one of the '-tx' options");
	}
	if (tar_action.option == NO_OPT)
	{
		errx(2, "a");
	}


	return tar_action;
}


/**
 * @brief Convert an octal string to a long in decimal format.
 * @param oct_str String representing octal number.
 * @return Decimal value of the octal number in string representation.
 */
long oct_to_dec(char* oct_str)
{
	long result = 0, divisor = 1;
	for (int i = strlen(oct_str)-1; i > 0; i--) 
	{
		result += (oct_str[i] - '0') * divisor;
		divisor *= 8;
	}
	return result;
}


/**
 * @brief Checks if BLOCKSIZE bytes of block are the null byte
 * @return 0 if true -1 otherwise.
 */
int is_empty(char* block)
{
	for (size_t i = 0; i < BLOCKSIZE; i++)
	{
		if (block[i] != '\0')
		{
			return 0;
		}
	}
	return -1;
}


void list_archive(char *archive_name, char** file_list, size_t file_list_len)
{
	FILE* fp;
	if ((fp = fopen(archive_name, "r")) == NULL)
	{
		errx(2, "%s: Cannot open: No such file or directory\nError is not recoverable: exiting now", archive_name);
	}
	
	// Allocate memory for the header
	struct posix_header* header = malloc(sizeof(struct posix_header)); 
	char* block = malloc(BLOCKSIZE);
	if (header == NULL || block == NULL)
	{
		errx(2, "Unable to allocate memory to read archive. Error is not recoverable: exiting now");
	}
	
	size_t bytes_read = 0;
	int num_zero_blocks = 0;

	while(1)
	{
		// Read the header
		bytes_read = fread(header, 1, BLOCKSIZE, fp);  
		
		if (bytes_read == 0)
		{
			break;
		}
		if (is_empty((char*)header))
		{
			num_zero_blocks++;
			break;
		}

		if (strcmp(header->magic, gnu_magic) != 0 && strcmp(header->magic, ustar_magic) != 0)
		{
			errx(2, "This does not look like a tar archive: %s", header->name);
		}
		if (header->typeflag != REGTYPE)
		{
			errx(2, "Unsupported header type: %d\n", header->typeflag);
		}

		// Check if the user has specified some files and if so
		// check if we have found a file the user is looking for.
		if (*file_list != NULL)
		{
			for (int i = file_list_len - 1; i >= 0; i--)
			{
				if (file_list[i] != NULL && strcmp(file_list[i], header->name) == 0)
				{
					// Remove the filename so we know we have found it already.
					file_list[i] = "";
					printf("%s\n", header->name);
					// otherwise unbuffered warnings get printed before as stderr is unbuffered.
					fflush(stdout);  
				}
			}
		}
		else
		{
			// If the user has not specified any files then we print all files.
			printf("%s\n", header->name);
			fflush(stdout);  
		}

		long size_in_bytes = oct_to_dec(header->size);
		int num_file_blocks = size_in_bytes / BLOCKSIZE;

		fseek(fp, (num_file_blocks - 1) * BLOCKSIZE, SEEK_CUR);
		bytes_read = fread(block, 1, BLOCKSIZE, fp);
		if (bytes_read != BLOCKSIZE)
		{
			warnx("Unexpected EOF in archive");
			errx(2, "Error is not recoverable: exiting now");
			
		}

	}
	bytes_read = fread(block, 1, BLOCKSIZE, fp);

	if (num_zero_blocks && bytes_read == 0)
	{
		warnx("A lone zero block at %ld", ftell(fp) / BLOCKSIZE);
	}

	// Checks if we didn't find any files the user looked for.
	int num_errors = 0;
	for (size_t i = 0; i < file_list_len; i++)
	{
		if (file_list[i] != NULL && strcmp(file_list[i], ""))
		{
			warnx("%s: Not found in archive", file_list[i]);
			num_errors++;
		}
	}
	if (num_errors != 0)
	{
		errx(2, "Exiting with failure status due to previous errors");
	}

	if (fclose(fp) != 0)
	{
		errx(2, "Error closing %s: exiting now", archive_name);
	}
	free(header);
}

int main(int argc, char** argv)
{
	struct tar_action_s operations = parse_args(argc, argv);
	
	switch (operations.option)
	{
	case LIST_OPT:
		list_archive(operations.filename, operations.file_list, operations.file_list_len);
		break;
	case EXTRACT_OPT:
		// TODO
		break;
	default:
		break;
	}
}
