#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fatal.h"

void fatal(int status, char *msg) {
    fwrite(msg, 1, strlen(msg), stderr);
    fwrite("\n", 1, 1, stderr);
    exit(status);
}
