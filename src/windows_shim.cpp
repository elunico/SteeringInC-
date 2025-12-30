#include "windows_shim.h"

//#ifdef WIN32

#include <chrono>
#include <thread>

void usleep_shim(long microseconds)
{
    std::this_thread::sleep_for(std::chrono::microseconds{microseconds});
}

extern "C" {

char const* optarg_shim = NULL;
int optopt_shim = '?';
int argpos = 1;

int getopt_shim(int argc, char const* argv[], char const* argstr)
{
    if (argpos == argc) {
        return -1; 
    }
    
    char const* arg     = argv[argpos];
    char const* checker = argstr;
    if (arg[0] != '-') {
        fprintf(stderr, "Warning: Argument provided without dash (%s) is not valid\n", arg);
        return '?';
    }
    while (*checker) {
        if (*checker == arg[1]) {
            if (*(checker + 1) == ':') {
                optarg_shim = argv[argpos + 1];
                if (optarg_shim == NULL) {
                    fprintf(stderr, "Warning: missing argument for option %s\n", arg);
                    // out of arguments, can return -1
                    return -1;
                }
                if (optarg_shim[0] == '-') {
                    fprintf(stderr, "Warning: %s expects an argument but next option was %s. Maybe you forgot the argument?\n", arg, optarg_shim);
                }
                argpos += 2;
            } else {
                argpos += 1;
            }
            return *checker;
        }
        checker++;
    }
    optopt_shim = arg[1];
    return '?';
}
}

//#endif
