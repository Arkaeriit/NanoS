#ifndef NANOS_H
#define NANOS_H

int main_nano(int argc, char **argv);
int main_st(int argc, char **argv);

void die_nano(const char *msg, ...);
void die_st(const char *msg, ...);

#define die(...) do {      \
    die_nano(__VA_ARGS__); \
    die_st(__VA_ARGS__);   \
} while(0)                  

extern int argc_nano;
extern char** argv_nano;

#endif

