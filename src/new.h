#ifndef KU_NEW_H
#define KU_NEW_H

#include <stdio.h>
#include <stdlib.h>

#define xnew(type) (type *)xmalloc(sizeof(type))

void *xmalloc(size_t size);

#endif // KU_NEW_H
