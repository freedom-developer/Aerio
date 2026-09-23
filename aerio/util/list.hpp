#ifndef AERIO_UTIL_LIST_HPP
#define AERIO_UTIL_LIST_HPP

#include <cstddef>
#include <type_traits>

namespace aerio {
namespace util {

struct list_head {
    struct list_head *prev, *next;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name) list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(list_head *list)
{
    list->prev = list;
    list->next = list;
}

static inline bool list_empty(const list_head *head)
{
    return head->next == head;
}

static inline void __list_add(list_head *_new, list_head *prev, list_head *next)
{
    next->prev = _new;
    _new->next = next;

    prev->next = _new;
    _new->prev = prev;
}

static inline void list_add(list_head *_new, list_head *head)
{
    __list_add(_new, head, head->next);
}

static inline void list_add_tail(list_head *_new, list_head *head)
{
    __list_add(_new, head->prev, head);
}

static inline void __list_del(list_head *prev, list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del_ini(list_head *entry)
{
    list_del(entry);
    INIT_LIST_HEAD(entry);
}

static inline bool list_is_head(const list_head *list, const list_head *head)
{
    return list == head;
}

#define list_entry(ptr, type, member)                                      \
    reinterpret_cast<type*>(                                               \
        reinterpret_cast<char*>(ptr) - offsetof(type, member))

#define list_entry_type(pos) \
    std::remove_cv_t<std::remove_reference_t<decltype(*(pos))>>

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, list_entry_type(pos), member)

#define list_first_entry(head, type, member) \
    list_entry((head)->next, type, member)
#define list_entry_is_head(pos, head, member) list_is_head(&pos->member, (head))

#define list_for_each_entry(pos, head, member) \
    for (pos = list_first_entry(head, list_entry_type(pos), member);      \
         !list_entry_is_head(pos, head, member);                         \
         pos = list_next_entry(pos, member))

}
}
#endif
