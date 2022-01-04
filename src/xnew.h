#ifndef KU_XNEW_H
#define KU_XNEW_H

#include <stdlib.h>

#define xnew(type) (type *)xmalloc(sizeof(type))

void *xmalloc(size_t size);

#endif // KU_XNEW_H
