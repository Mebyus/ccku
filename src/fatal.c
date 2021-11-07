#include <stdio.h>
#include <stdlib.h>

#include "fatal.h"

void fatal(int status, char *msg) {
    printf("%s\n", msg);
    exit(status);
}
