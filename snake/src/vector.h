/*
This vector struct is no longer used in the program. It was created as an
exercise for the author and as a reference for future programs.
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct _vector vector;

vector *vector_init(size_t initialCapacity);
void vector_add(vector *vec, void *elem);
void *vector_get(vector *vec, size_t index);
void vector_free(vector *vec);

#endif
