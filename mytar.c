#include <stdio.h>
#include <err.h>

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

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */

enum Option 
{
	NO_OPT,
	EXTRACT_OPT,
	LIST_OPT
};

/**
 * @brief Finds and returns cmd-line options.
 *
 * Walks the given pointer until an operation or option [word starting with '-']
 * is found  and returns the character following '-' or -1 upon walking to 
 * the end of the args array.
 * 
 * @param argv Command-line arguments.
 * @return int Character code of option or -1 at end of input.
 */
int get_opt(char*** argv)
{
	while (NULL != **argv)
	{
		if (***argv == '-')
		{
			char opt = *(**argv + 1);
			(*argv)++;
			return opt;
		}
		(*argv)++;
	}
	return -1;
}

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

	if (argc == 1) 
	{
		err(2, "need at least one option");
	}

	int opt;
	while ((opt = get_opt(&argv)) != -1)
	{
		switch (opt)
		{
			case 't':
				for (int i = 0; NULL != *argv && **argv != '-'; i++)
				{
					tar_action.file_list[i] = *argv;	
					argv++;
				}
				tar_action.option = LIST_OPT;
				break;
			case 'f':
				/**
				 * If it is specified then the next word in
				 * arguments is the filename.
				 */
				if (NULL == *argv)
				{
					err(64, "option requires an argument -- 'f'");
				}
				tar_action.filename = *argv;
				break;
			default:
				err(2, "Unknown option -%c", opt);
		}
	}

	if (NULL != tar_action.filename && tar_action.option == NO_OPT) 
	{
		err(2, "You must specify one of the '-tx' options");
	}

	return tar_action;
}


int main(int argc, char** argv)
{
	struct tar_action_s operations = parse_args(argc, argv);
}
