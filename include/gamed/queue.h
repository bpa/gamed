#ifndef GAMED_QUEUE
#define GAMED_QUEUE

#include <sys/queue.h>

#ifndef LIST_FIRST
#define LIST_FIRST(head)    ((head)->lh_first)
#endif

#ifndef LIST_NEXT
#define LIST_NEXT(elm, field)   ((elm)->field.le_next)
#endif

#ifndef LIST_FOREACH
#define LIST_FOREACH(var, head, field)                  \
        for ((var) = LIST_FIRST((head));                \
                        (var);                          \
                        (var) = LIST_NEXT((var), field))
#endif

#ifndef LIST_FOREACH_SAFE
#define LIST_FOREACH_SAFE(var, head, field, tvar)           \
    for ((var) = LIST_FIRST((head));                        \
            (var) && ((tvar) = LIST_NEXT((var), field), 1); \
            (var) = (tvar))
#endif

#endif
