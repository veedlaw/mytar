#include <stdio.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>

// TODO: Implement
//  * -t operation
//      (also known as --list)
//      Lists the contents of an archive, or with args
//  * -f option
// 

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
	struct tar_action_s tar_action;
	tar_action.file_list = malloc(argc * sizeof(char*));

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

	return tar_action;
}


int main(int argc, char** argv)
{
	struct tar_action_s operations = parse_args(argc, argv);
}
