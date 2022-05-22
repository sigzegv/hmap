#include "hmap.h"

uint32_t
murmur_hash(const void* key) {
    uint32_t h = 0x12345678; // Seed
    const uint8_t* str = key;

    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }

    return h;
}

/**
 * create new hmap node
 */
hmap_node_t*
hmap_node_new(char* key, void* data) {
    hmap_node_t* n = malloc(sizeof(hmap_node_t));
    if (NULL == n) {
        return NULL;
    }

    n->key = malloc(sizeof(char) * strlen(key) + 1);
    strcpy(n->key, key);
    n->data = data;
    n->prev = NULL;
    n->next = NULL;

    return n;
}

/**
 * appends a node to node chain
 */
hmap_node_t*
hmap_node_append(hmap_node_t* current, hmap_node_t* node) {
    if (NULL == current) {
        return NULL;
    }

    // move to last node in chain
    for (; NULL != current->next; current = current->next);

    node->prev = current;
    current->next = node;

    return node;
}

/**
 * frees node resources and rebuild the node chain
 */
void
hmap_node_free(hmap_node_t* node) {
    if (NULL != node->prev) {
        node->prev->next = node->next;
    }

    if (NULL != node->next) {
        node->next->prev = node->prev;
    }
    
    free(node->key);
    free(node);
    node = NULL;
}

uint32_t
hmap_gen_key(hmap_t* hmap, const void * key) {
    uint32_t h = hmap->key_gen_fn(key);
    return h % hmap->size;
}

hmap_t*
hmap_new(size_t sz) {
    hmap_t* h = malloc(sizeof(hmap_t));
    if (NULL == h) {
        return NULL;
    }

    h->size = sz;
    h->filled = 0;
    h->entry = malloc(sizeof(hmap_node_t*) * h->size);
    h->key_gen_fn = murmur_hash;

    if (NULL == h->entry) {
        free(h);
        return NULL;
    }

    // sets every node entry to NULL
    for (int i=0; i < h->size; i++) {
        hmap_node_t** n = h->entry + i;
        *n = NULL;
    }

    return h;
}

float
hmap_get_load(hmap_t* h) {
    return h->filled / h->size;
}

int
hmap_resize(hmap_t** old_h) {
    hmap_t* new_h = hmap_new((*old_h)->size * 2);
    if (NULL == new_h) {
        return 1;
    }

    // move every node in new map
    hmap_iter_t* it = hmap_iter_new(*old_h);
    for (hmap_node_t* n = hmap_iter_next(it); n != NULL; n = hmap_iter_next(it)) {
        hmap_set(new_h, n->key, n->data);
    }
    free(it);

    hmap_free(*old_h);
    *old_h = new_h;

    return 0;
}

int
hmap_set(hmap_t* h, char *key, void* data) {
    uint32_t k = hmap_gen_key(h, key);
    hmap_node_t** e = h->entry + k;

    if (NULL == *e) {
        *e = hmap_node_new(key, data);
        if (NULL == *e) {
            return 1;
        }
    } else {
        hmap_node_t* n = hmap_node_new(key, data);
        if (NULL == n) {
            return 1;
        }

        hmap_node_append(*e, n);
    }

    ++h->filled;

    return 0;
}

hmap_node_t*
hmap_find_node(hmap_t* h, char *key) {
    uint32_t k = hmap_gen_key(h, key);
    hmap_node_t** n = h->entry + k;

    if (NULL == *n) {
        return NULL;
    }

    hmap_node_t* node = *n;

    do { // search key in current key chain
         if (0 == strcmp(key, node->key)) {
            return node;
         }
         node = node->next;
    } while(NULL != node);

    return NULL;
}

void*
hmap_unset(hmap_t* h, char* key) {
    uint32_t k = hmap_gen_key(h, key);
    hmap_node_t** n = h->entry + k;
    hmap_node_t* node = hmap_find_node(h, key);

    if (NULL == node) {
        return NULL;
    }

    if (NULL == node->prev) {
        *n = node->next;
    }

    void* data = node->data;

    // clear node
    hmap_node_free(node);

    --h->filled;

    return data;
}

void*
hmap_get(hmap_t* h, char *key) {
    hmap_node_t* n = hmap_find_node(h, key);
    if (NULL == n) {
        return NULL;
    }

    return n->data;
}

void
hmap_free(hmap_t *h) {
    hmap_iter_t* it = hmap_iter_new(h);
    for (hmap_node_t* n = hmap_iter_next(it); n != NULL; n = hmap_iter_next(it)) {
        hmap_unset(h, n->key);
    }

    free(it);
    free(h->entry);
    free(h);
}

hmap_iter_t*
hmap_iter_new(hmap_t* h) {
    hmap_iter_t* i = (hmap_iter_t*)malloc(sizeof(hmap_iter_t));
    if (NULL == i) {
        return NULL;
    }

    i->hmap = h;
    i->offset = 0;
    i->cnode = NULL;

    return i;
}

hmap_node_t*
hmap_iter_next(hmap_iter_t* it) {
    if (it->offset >= it->hmap->size) {
        return NULL;
    }

    if (NULL != it->cnode && NULL != it->cnode->next) {
        it->cnode = it->cnode->next;
        return it->cnode;
    }

    hmap_node_t* current = NULL;
    do {
        if (it->offset >= it->hmap->size) {
            return NULL;
        }

        current = *(it->hmap->entry + it->offset);
        it->cnode = current;
        ++it->offset;
    } while(NULL == current);

    return current;
}
