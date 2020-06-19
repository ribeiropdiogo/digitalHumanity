#ifndef _MKFRT_DICTIONARY_H_
#define _MKFRT_DICTIONARY_H_

#include <glib.h>

typedef struct dictionary * Dictionary;

Dictionary makeDictionary(GDestroyNotify value_destroy_fun);

gpointer getValueDictionary(Dictionary dict, char *key);

int containsDictionary(Dictionary dict, char *key);

int insertDictionary(Dictionary dict, char *key, gpointer value);

int removeDictionary(Dictionary dict, char *key);

void foreachDictionary(Dictionary dict, GHFunc func, void *data);

void appendDictionary(Dictionary dict, char *key, gpointer value);

int sizeDictionary(Dictionary dict);

void destroyDictionary(Dictionary dict);

#endif
