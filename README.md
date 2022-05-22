# C Hashmap implementation

Implements a hashmap with string as key.
It uses Murmur hash algorithm to generate indexes from keys.

## TODO
* allow custom key generation function
* allow any type as key
* add replace option for existing keys

## How to use

```
    typedef struct {
        ...
    } data_t;

    [...]

    // misc data
    data_t *d1 = malloc(sizeof(data_t));
    data_t *d2 = malloc(sizeof(data_t));
    data_t *d3 = malloc(sizeof(data_t));

    // create a hashmap
    hmap_t *h = hmap_new(64);
    if (NULL == h) {
        fprintf(stderr, "hmap_new failed\n");
        return;
    }

    // set somes keys, returns 0 on failure, 1 on success
    hmap_set(h, "key1", d1);
    hmap_set(h, "key2", d2);
    hmap_set(h, "keyX", d3);

    // get some data
    data_t* d = (data_t*)hmap_get(h, "key2");
    if (NULL == d) {
        fprintf(stderr, "hmap_get: key2 not found\n");
        return;
    }

    // unset a key
    hmap_unset(h, "keyX");

    hmap_free(h);
```

See *hmap_test.c* for more examples.
