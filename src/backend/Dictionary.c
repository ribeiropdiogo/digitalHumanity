#include <glib.h>
#include "Dictionary.h"

typedef struct dictionary {
	GHashTable *table;
} *Dictionary;

Dictionary makeDictionary() {
	Dictionary dict = malloc(sizeof(struct dictionary));

	dict->table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	return dict;
}

gpointer getValueDictionary(Dictionary dict, char *key) {
	return g_hash_table_lookup(dict->table, (gpointer)key);
}

int containsDictionary(Dictionary dict, char *key) {
	return g_hash_table_contains(dict->table, key);
}

int insertDictionary(Dictionary dict, char *key, gpointer value) {
	int r = 1;

	if(!g_hash_table_contains(dict->table, key)) {
		g_hash_table_insert(dict->table, key, value);
	}
	else
		r = 0;

	return r;
}

void appendDictionary(Dictionary dict, char *key, gpointer value) {
	g_hash_table_insert(dict->table, key, value);
}

int removeDictionary(Dictionary dict, char *key) {
	return (int)g_hash_table_remove(dict->table, key);
}

void foreachDictionary(Dictionary dict, GHFunc func) {
	g_hash_table_foreach(dict->table, func, NULL);
}

int sizeDictionary(Dictionary dict) {
	return g_hash_table_size(dict->table);
}

void destroyDictionary(Dictionary dict) {
	g_hash_table_destroy(dict->table);
	free(dict);
}