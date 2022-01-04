#include "xnew.h"
#include "fatal.h"
#include "types.h"


void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == nil) {
        fatal(1, "not enough memory");
    }
    return ptr;
}
