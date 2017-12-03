/* mat_string.h : defining structures and basic matrix <> string functions */

#define Version "2.5"

/* unidirectional string chaining */

struct Elt {
     void * obj; /* untyped address of object */
     struct Elt * fol; /* address of next, NULL if end */
};

#define END (struct Elt*)NULL

extern void init_string(void);
extern struct Elt * add_string(void * objet);
extern int del_string(void * objet);
extern void list_string(void);
extern void * last_addr_string(void);


