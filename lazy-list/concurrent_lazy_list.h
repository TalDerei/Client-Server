/**
 * @file lazy_list.h
 * 
 * Lazy list implementation
 */

#ifndef LAZY_LIST_DEF
#define LAZY_LIST_DEF

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define VAL_MIN INT_MIN
#define VAL_MAX INT_MAX

using namespace std;

typedef pthread_mutex_t ptlock_t;
#  define INIT_LOCK(lock)				pthread_mutex_init((pthread_mutex_t *) lock, NULL);
#  define DESTROY_LOCK(lock)			pthread_mutex_destroy((pthread_mutex_t *) lock)
#  define LOCK(lock)					pthread_mutex_lock((pthread_mutex_t *) lock)
#  define UNLOCK(lock)					pthread_mutex_unlock((pthread_mutex_t *) lock)


template <typename K, typename V>
class lazyList {
    /** Typedef for integer pointer */
    typedef intptr_t val_t;

    /** node_l_t struct represents a node in lazy list */
    typedef struct node_l {
        val_t key;
        val_t val;
        struct node_l *next;
        volatile ptlock_t lock;
    } node_l_t;

    /** intset_l struct represents head of lazy list */
    typedef struct intset_l {
        node_l_t *head;
    } intset_l_t;

    /** Pointer to intset_l struct */
    intset_l_t *set;

public: 

/** Default constructor */
lazyList() {}

/** Initialize lazy list */
void initialize() {
    cout << "Initializing lazy list!" << endl;
    set = set_new_l();
}

node_l_t *new_node_l(val_t key, val_t val, node_l_t *next, int transactional) {
    node_l_t *node_l;
    node_l = (node_l_t *)malloc(sizeof(node_l_t));
    if (node_l == NULL) {
        perror("malloc");
        exit(1);
    }
    node_l->key = key;
    node_l->val = val;
    node_l->next = next;
    INIT_LOCK(&node_l->lock);	

    return node_l;
}

intset_l_t *set_new_l() {
    intset_l_t *set;
    node_l_t *min, *max;

    if ((set = (intset_l_t *)malloc(sizeof(intset_l_t))) == NULL) {
        perror("malloc");
        exit(1);
    }
    max = new_node_l(VAL_MAX, VAL_MAX, NULL, 0);
    min = new_node_l(VAL_MIN, VAL_MIN, max, 0);
    set->head = min;

    return set;
}

void node_delete_l(node_l_t *node) {
    DESTROY_LOCK(&node->lock);
    free(node);
}

void set_delete_l() {
    node_l_t *node, *next;

    node = set->head;
    while (node != NULL) {
        next = node->next;
        DESTROY_LOCK(&node->lock);
        free(node);
        node = next;
    }
    free(set);
}

int set_size_l(intset_l_t *set) {
    int size = 0;
    node_l_t *node;

    /* We have at least 2 elements */
    node = set->head->next;
    while (node->next != NULL) {
        size++;
        node = node->next;
    }

    return size;
}

inline int is_marked_ref(long i) {
    return (int) (i &= LONG_MIN+1);
}

inline long unset_mark(long i) {
    i &= LONG_MAX-1;
    return i;
}

inline long set_mark(long i) {
    i = unset_mark(i);
    i += 1;
    return i;
}

inline node_l_t *get_unmarked_ref(node_l_t *n) {
    return (node_l_t *) unset_mark((long) n);
}

inline node_l_t *get_marked_ref(node_l_t *n) {
    return (node_l_t *) set_mark((long) n);
}

/*
* Checking that both curr and pred are both unmarked and that pred's next pointer
* points to curr to verify that the entries are adjacent and present in the list.
*/
inline int parse_validate(node_l_t *pred, node_l_t *curr) {
    return (!is_marked_ref((long) pred->next) && !is_marked_ref((long) curr->next) && (pred->next == curr));
}

std::pair<int, int> parse_find(val_t key) {
    node_l_t *curr;
    curr = set->head;
    while (curr->key < key)
        curr = get_unmarked_ref(curr->next);
    return {curr->val, ((curr->key == key) && !is_marked_ref((long) curr->next))};
}

int parse_insert(val_t key, val_t val) {
    node_l_t *curr, *pred, *newnode;
    int result, validated, notVal;
    
    while (1) {
        pred = set->head;
        curr = get_unmarked_ref(pred->next);
        while (curr->key < key) {
            pred = curr;
            curr = get_unmarked_ref(curr->next);
        }
        LOCK(&pred->lock);
		LOCK(&curr->lock);
        validated = parse_validate(pred, curr);
        notVal = (curr->key != key);
        result = (validated && notVal);
        if (result) {
            newnode = new_node_l(key, val, curr, 0);
            pred->next = newnode;
        } 
        UNLOCK(&curr->lock);
		UNLOCK(&pred->lock);
        if(validated)
            return result;
    }
}

/*
* Logically remove an element by setting a mark bit to 1 
* before removing it physically.
*
* NB. it is not safe to free the element after physical deletion as a 
* pre-empted find operation may currently be parsing the element.
* TODO: must implement a stop-the-world garbage collector to correctly 
* free the memory.
*/
int parse_delete(val_t key) {
    node_l_t *pred, *curr;
    int result, validated, isVal;
    while(1) {
        pred = set->head;
        curr = get_unmarked_ref(pred->next);
        while (curr->key < key) {
            pred = curr;
            curr = get_unmarked_ref(curr->next);
        }
        LOCK(&pred->lock);
		LOCK(&curr->lock);
        validated = parse_validate(pred, curr);
        isVal = key == curr->key;
        result = validated && isVal;
        if (result) {
            curr->next = get_marked_ref(curr->next);
            pred->next = get_unmarked_ref(curr->next);
        }
        UNLOCK(&curr->lock);
		UNLOCK(&pred->lock);
        if(validated)
            return result;
    }
}
};

#endif