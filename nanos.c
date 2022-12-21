#include "nanos.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define FLAG_FOR_ST(flag)                  \
    else if(!strcmp(argv_in[i], flag)) {   \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        continue;                          \
    }                                       

#define OPTION_FOR_ST(flag)                \
    else if(!strcmp(argv_in[i], flag)) {   \
        if (i == argc_in - 1) {            \
            return 1;                      \
        }                                  \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        i++;                               \
        (*argv_st)[*argc_st] = argv_in[i]; \
        (*argc_st)++;                      \
        continue;                          \
    }                                       


static int sort_args(int argc_in, char** argv_in, int* argc_st, char*** argv_st, int* argc_nano, char*** argv_nano) {
	*argc_st = 0;
	*argc_nano = 0;
	for (int i=0; i<argc_in; i++) {
		if (0) {
			// Need a false entry for the following macros to work
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
	return 0;
}

int argc_nano;
char** argv_nano;

int main(int argc, char** argv) {
	argv_nano = malloc(sizeof(char*) * argc);
	char** argv_st = malloc(sizeof(char*) * argc);
	int argc_st;
	sort_args(argc, argv, &argc_st, &argv_st, &argc_nano, &argv_nano);
	return main_st(argc_st, argv_st);
}

