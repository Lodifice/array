#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define array(type, name, initial_length) \
    type *name = __array_alloc(sizeof(type), initial_length)

#define aforeach(it, array) \
    for (unsigned it = 0; \
            it < alength(array); \
            ++it)

#define __header(array) \
    ((struct __array_header *) array - 1)

#define alength(array) \
    (__header(array)->length)

#define afree(array) \
    free(__header(array))

#define apush(array, elem) \
    __array_resize((void **) &array, sizeof *array, 1); \
    array[alength(array)-1] = elem

#define apop(array) \
    aremove(array, (alength(array) - 1))

#define aremove(array, index) \
    assert(alength(array) > index); \
    memmove(array + index, array + index + 1, sizeof *array * (alength(array) - index - 1)); \
    __array_resize((void **) &array, sizeof *array, -1)

#define ainsert(array, index, elem) \
    __array_resize((void **) &array, sizeof *array, index >= alength(array) ? index - alength(array) + 1 : 1); \
    memmove(array + index + 1, array + index, sizeof *array * (alength(array) - index - 1)); \
    array[index] = elem

#define acontains(array, elem) \
    __array_search(array, &elem, sizeof elem)

#define __arrayallocated(array) \
    (__header(array)->allocated)

struct __array_header {
    unsigned length;
    unsigned allocated;
};

unsigned __bump_up(unsigned n) {
    unsigned base = 1;
    --n;
    while (base < sizeof n * 8) {
        n |= n >> base;
        base *= 2;
    }
    ++n;
    n += (n == 0);
    return n;
}

void *__array_alloc(size_t size, unsigned length) {
    unsigned allocated = __bump_up(length);
    struct __array_header *head = malloc(sizeof *head + allocated * size);
    assert(head);
    head->length = length;
    head->allocated = allocated;
    return (void *) (head + 1);
}

void __array_resize(void **array, size_t size, int difference) {
    if (difference == 0) {
        return;
    }
    struct __array_header *head = __header(*array);
    head->length += difference;
    if (head->length >= head->allocated) {
        head->allocated = __bump_up(head->length);
        head = realloc(head, sizeof *head + head->allocated * size);
        assert(head);
        *array = head + 1;
    }
}

int __array_search(void *array, void *elem, size_t size) {
    for (unsigned i = 0; i < alength(array) * size; i += size) {
        if (memcmp(array + i, elem, size) == 0) {
            return 1;
        }
    }
    return 0;
}
