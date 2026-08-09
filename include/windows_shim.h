//#ifdef WIN32

// define our own usleep and getopt
extern "C" {

void usleep_shim(long microseconds);

extern char const* optarg_shim;
extern int argpos;
extern int optopt_shim; 

int getopt_shim(int argc, char const *argv[], char const* argstr);

}

//#else

//#include <unistd.h>
// use built-in POSIX functions

//#endif
