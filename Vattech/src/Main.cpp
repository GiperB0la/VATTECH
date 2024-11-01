#include "../include/Work.h"


int main(int argc, char* argv[])
{
	Work w;
    if (argc > 1) {
        w.start(argv[1]);
    }
    else {
        w.start();
    }

	return 0;
}