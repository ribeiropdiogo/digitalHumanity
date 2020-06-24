#include <glib.h>
#include "TupleSet.h"

typedef struct tuple_dictionary {
        GHashTable *table;
} *TupleSet;

char *merge_keys(char *key1, char *key2);

TupleSet makeTupleSet() {
        TupleSet set = malloc(sizeof(struct tuple_dictionary));

        set->table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

        return set;
}

int containsTupleSet(TupleSet set, char *key1, char *key2) {
        char *code = merge_keys(key1, key2);

        int res = g_hash_table_contains(set->table, code);

        g_free(code);

        return res;

}

int insertTupleSet(TupleSet set, char *key1, char *key2) {
        char *code = merge_keys(key1, key2);

        return g_hash_table_insert(set->table, code, NULL);
}

int removeTupleSet(TupleSet set, char *key1, char *key2) {
        char *code = merge_keys(key1, key2);

        int res = g_hash_table_remove(set->table, code);

        g_free(code);

        return res;
}

void foreachTupleSet(TupleSet set, GHFunc func, gpointer user_data) {
        g_hash_table_foreach(set->table, func, user_data);
}

int sizeTupleSet(TupleSet set) {
        return g_hash_table_size(set->table);
}

void destroyTupleSet(gpointer vset) {
        TupleSet set = (TupleSet)vset;
        g_hash_table_destroy(set->table);
        free(set);
}

char *merge_keys(char *key1, char *key2) {
        GString * string = g_string_new(NULL);

        g_string_printf(string, "%s<->%s", key1, key2);

        return g_string_free(string, FALSE);
}

char *get_keys(char *code, char **ekey1, char **ekey2) {
        char *cp = strdup(code);
        char *sep = strstr(cp, "<->");

        *sep = '\0';
        *ekey1 = cp;
        *ekey2 = sep + 3;

        return cp;
}
