#include <stdio.h>
#include <err.h>

// TODO: Implement
//  * -t operation
//      (also known as --list)
//      Lists the contents of an archive, or with args
//  * -f option
// 


enum Option {
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


void parse_args(int argc, char** argv)
{
	char* filename;
	enum Option operation = NO_OPT;
	char* list_files[argc];

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
					list_files[i] = *argv;	
					argv++;
				}
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
				filename = *argv;
				break;
			default:
				err(2, "Unknown option -%c", opt);
		}
	}

	if (NULL != filename && operation == NO_OPT)
	{
		err(2, "You must specify one of the '-tx' options");
	}
}


int main(int argc, char** argv)
{
	parse_args(argc, argv);
}
