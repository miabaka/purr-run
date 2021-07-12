#pragma once

#include <stddef.h>
#include <malloc.h>

#define DEFINE_LIST(name, itemType)       \
    typedef struct name##Item name##Item; \
                                          \
    struct name##Item {                   \
        name##Item *next;                 \
        itemType value;                   \
    };                                    \
                                          \
    typedef struct {                      \
        name##Item *firstItem;            \
        name##Item *lastItem;             \
    } name;                               \
                                          \
    void name##_init(name *this);         \
                                          \
    void name##_destroy(name *this);      \
                                          \
    itemType *name##_newItem(name *this); \

#define IMPLEMENT_LIST(name, itemType)                 \
    void name##_init(name *this) {                     \
        this->firstItem = NULL;                        \
        this->lastItem = NULL;                         \
    }                                                  \
                                                       \
    void name##_destroy(name *this) {                  \
        name##Item *item = this->firstItem;            \
                                                       \
        while (item) {                                 \
            name##Item *nextItem = item->next;         \
                                                       \
            free(item);                                \
            item = nextItem;                           \
        }                                              \
    }                                                  \
                                                       \
    itemType *name##_newItem(name *this) {             \
        name##Item *item = malloc(sizeof(name##Item)); \
                                                       \
        item->next = NULL;                             \
                                                       \
        if (this->lastItem) {                          \
            this->lastItem->next = item;               \
            this->lastItem = item;                     \
        } else {                                       \
            this->firstItem = item;                    \
            this->lastItem = item;                     \
        }                                              \
                                                       \
        return &item->value;                           \
    }

#define LIST_FOR(list, it) \
    for (typeof((list).firstItem) (it) = (list).firstItem; (it); (it) = ((it)->next))

#define LIST_PFOR(list, it) \
    for (typeof((list).firstItem) (it) = (list).firstItem, (it##prev) = NULL; (it); (it) = ((it)->next))

#define LIST_PFOR_REMOVE(list, it)                      \
    {                                                   \
        typeof((list).firstItem) it##next = (it)->next; \
                                                        \
        free(it);                                       \
                                                        \
        if (it##prev)                                   \
            it##prev->next = it##next;                  \
                                                        \
        if ((it) == (list).firstItem)                   \
            (list).firstItem = it##next;                \
                                                        \
        if ((it) == (list).lastItem)                    \
            (list).lastItem = it##next;                 \
                                                        \
        if (!it##next)                                  \
            break;                                      \
                                                        \
        (it) = it##next;                                \
    }

#define LIST_PFOR_TAIL(it) (it##prev) = (it);

#define LIST_ITERATOR_VALUE(it) it->value