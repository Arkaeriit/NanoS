#include "nanos.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "pictures.h"

extern const char **colorname;

#define FLAG_FOR_ST(flag)                  \
    else if(!strcmp(argv_in[i], flag)) {   \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        continue;                          \
    }                                       

#define OPTION_FOR_ST(flag)                \
    else if(!strcmp(argv_in[i], flag)) {   \
        if (i == argc_in - 1) {            \
            return MISSING_OPTION;         \
        }                                  \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        i++;                               \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        continue;                          \
    }                                       

enum sort_arg_result {
	OK = 0,
	MISSING_OPTION,
	VERSION,
	HELP,
};

static enum sort_arg_result sort_args(int argc_in, char** argv_in, int* argc_st, char*** argv_st, int* argc_nano, char*** argv_nano) {
	*argc_st = 1;
	*argc_nano = 1;
	(*argv_st)[0] = argv_in[0];
	(*argv_nano)[0] = argv_in[0];
	for (int i=1; i<argc_in; i++) {
		if (!strcmp(argv_in[i], "help") || !strcmp(argv_in[i], "--help") || !strcmp(argv_in[i], "-help") || !strcmp(argv_in[i], "-h")) {
			return HELP;
		} else if (!strcmp(argv_in[i], "--version")) {
			return VERSION;
		}
		FLAG_FOR_ST("-a")
		FLAG_FOR_ST("-i")
		OPTION_FOR_ST("-c")
		OPTION_FOR_ST("-f")
		OPTION_FOR_ST("-g")
		OPTION_FOR_ST("-n")
		OPTION_FOR_ST("-t")
		OPTION_FOR_ST("-T")
		OPTION_FOR_ST("-w")
		OPTION_FOR_ST("-l")
		else {
			(*argv_nano)[*argc_nano] = argv_in[i];
			(*argc_nano)++;
		}
	}
	return OK;
}

int argc_nano;
char** argv_nano;

int main(int argc, char** argv) {
	argv_nano = malloc(sizeof(char*) * argc);
	char** argv_st = malloc(sizeof(char*) * argc);
	int argc_st;
	enum sort_arg_result sa_rc = sort_args(argc, argv, &argc_st, &argv_st, &argc_nano, &argv_nano);
	switch (sa_rc) {
		case OK:
			colorname = picture_get_st_theme();
			int rc = main_st(argc_st, argv_st);
			die("Exiting\n");
			return rc;
		case MISSING_OPTION:
			fprintf(stderr, "Error, missing argument for option \"%s\".\n", argv[argc-1]);
			return 1;
		case VERSION:
			printf("TODO: branding\n");
			return 0;
		case HELP:
			printf("TODO: help\n");
			return 0;
	}
}

