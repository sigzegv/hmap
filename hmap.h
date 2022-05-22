#ifndef HMAP_H
#define HMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct _hmap_node_t {
    char* key;
    void* data;
    struct _hmap_node_t* prev;
    struct _hmap_node_t* next;
} hmap_node_t;

typedef struct {
    size_t size;
    size_t filled;
    hmap_node_t** entry;
} hmap_t;

uint32_t hmap_gen_key(const void*, size_t);
hmap_t* hmap_new(size_t);
float hmap_get_load(hmap_t*);
void hmap_resize(hmap_t*);
int hmap_set(hmap_t*, char*, void*);
hmap_node_t* hmap_find_node(hmap_t*, char *);
void* hmap_unset(hmap_t*, char*);
void* hmap_get(hmap_t*, char *key);
void hmap_free(hmap_t*);

typedef struct {
    hmap_t *hmap;
    int offset;
    hmap_node_t* cnode;
} hmap_iter_t;

hmap_iter_t* hmap_iter_new(hmap_t*);
hmap_node_t* hmap_iter_next(hmap_iter_t*);

#endif
