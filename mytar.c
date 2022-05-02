#include <stdio.h>
#include <err.h>

// TODO: Implement
//  * -t operation
//      (also known as --list)
//      Lists the contents of an archive, or with args
//  * -f option
// 


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
	enum Option operation = NO_OPT;
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
				operation = LIST_OPT;
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

	if (NULL != tar_action.filename && operation == NO_OPT) 
	{
		err(2, "You must specify one of the '-tx' options");
	}

	return tar_action;
}


int main(int argc, char** argv)
{
	struct tar_action_s operations = parse_args(argc, argv);
}
