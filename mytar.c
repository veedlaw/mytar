#include <stdio.h>
#include <err.h>

// TODO: Implement
//  * -t operation
//      (also known as --list)
//      Lists the contents of an archive, or with args
//  * -f option
// 

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
	char** list_files;

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
				// TODO
				break;
			case 'f':
				// TODO
				break;
			default:
				err(2, "Unknown option -%c", opt);
		}
	}
}


int main(int argc, char** argv)
{
	parse_args(argc, argv);
}
