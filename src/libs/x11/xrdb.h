#ifndef XRDB_H
#define XRDB_H
/* See LICENSE file for copyright and license details. */

#if XRESOURCES
enum resource_type {
    STRING = 0,
    INTEGER = 1,
    FLOAT = 2
};

typedef struct {
    char *name;
    enum resource_type type;
    void *dst;
} ResourcePref;

// declare functions
static void load_xresources(void);
static void resource_load(XrmDatabase db, char *name, enum resource_type rtype, void *dst);
#endif
#endif
