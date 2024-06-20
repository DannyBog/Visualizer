#ifndef BOG_MAP_H
#define BOG_MAP_H

#include <assert.h>
#include <stdlib.h>

#define BOG_ARRAY_LIST(type) \
struct {type *at; size_t len, cap;}

#define SET_LEN(a,n) ((a).len = (n), SET_CAP((a), (a).len))
#define SET_CAP(a,n) ((a).cap < (n) ? \
((a).cap <<= 1, (((n) > (a).cap) ? (a).cap = (n) : 0), \
(a).at = realloc((a).at, (a).cap * sizeof(*(a).at))) : 0)

#define BOG_ADD_ELEMENT(a,v) (SET_LEN((a), (a).len + 1), (a).at[(a).len - 1] = (v))
#define BOG_REMOVE_ELEMENT(a,i) (assert((i) <= (a).len && (i) >= 0), memcpy((a).at + (i), (a).at + (i) + 1, ((a).len - (i) - 1) * sizeof(*(a).at)), SET_LEN((a), (a).len - 1))
#define BOG_CLEAR_LIST(a) (free((a).at), (a).at = 0, (a).len = (a).cap = 0)

#endif //BOG_MAP_H