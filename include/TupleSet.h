#ifndef _MKFRT_TUPLE_SET_H_
#define _MKFRT_TUPLE_SET_H_

#include <glib.h>

typedef struct tuple_dictionary * TupleSet;

TupleSet makeTupleSet();

int containsTupleSet(TupleSet dict, char *key1, char *key2);

int insertTupleSet(TupleSet dict, char *key1, char *key2);

int removeTupleSet(TupleSet dict, char *key1, char *key2);

void foreachTupleSet(TupleSet dict, GHFunc func, gpointer user_data);

int sizeTupleSet(TupleSet dict);

void destroyTupleSet(gpointer dict);

char *get_keys(char *code, char **ekey1, char **ekey2);

#endif
