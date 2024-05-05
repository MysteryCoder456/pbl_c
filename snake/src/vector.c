#include "vector.h"
#include <stdlib.h>

struct _vector {
    size_t capacity;
    size_t size;
    void **elements;
};

vector *vector_init(size_t initialCapacity) {
    vector *vec = (vector *)malloc(sizeof(vector));

    vec->size = 0;
    vec->capacity = initialCapacity;
    vec->elements = (void **)calloc(vec->capacity, sizeof(void *));

    return vec;
}

void vector_add(vector *vec, void *elem) {
    if (vec->size == vec->capacity) {
        // Reallocate elements array
        vec->capacity *= 2;
        vec->elements = realloc(vec->elements, vec->capacity * sizeof(void *));
    }

    vec->elements[vec->size] = elem;
    vec->size++;
}

void *vector_get(vector *vec, size_t index) {
    if (index >= vec->size)
        return NULL;
    return vec->elements[index];
}

void vector_free(vector *vec) {
    // Free elements
    for (int i = 0; i < vec->size; i++) {
        void *elem = vec->elements[i];
        free(elem);
    }

    // Free vector struct
    free(vec->elements);
    free(vec);
}
