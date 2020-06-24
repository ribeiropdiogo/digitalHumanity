#include <stdio.h>
#include "Manager.h"

int contexto;

typedef struct manager {
        /*
           Associa a cada tópico o seu significado
           textual.

           Contem valores do tipo `char *`.
         */
        Dictionary descriptions;

        /*
           Associa a cada topico o seu titulo.

           contem valores do tipo `char *`.
         */
        Dictionary titles;

        /*
           Associa a cada relação de inter-relações, a
           respectiva função de tratamento.

           Contem valores do tipo `TreatFunc`.
         */
        Dictionary relation_treatment;

        /*
           Associa a cada relação o conjunto de tuplos
           que tiram partido dessa relacao.

           Contem valores do tipo `TupleSet`, onde este
           é constituido por vários elementos do tipo
           `<sujeito, objeto>`.
         */
        Dictionary free_relations;

        /*
           Associa a cada relação o conjunto de tuplos
           que tiram partido dessa.
         */
        Dictionary reserved_relations;

        /*
           Associa a cada relação inter-relações o conjunto
           de tuplos que tiram partido desta.
         */
        Dictionary inter_relations;
} *Manager;

void synonym_foreach(gpointer key, gpointer value,
                     gpointer user_data);

void inverseof_foreach(gpointer key, gpointer value,
                       gpointer user_data);

void intreat_foreach(gpointer key, gpointer value,
                     gpointer user_data);

void treatment_foreach(gpointer key, gpointer value,
                       gpointer user_data);

void dump_free_rel(gpointer key, gpointer value,
                   gpointer user_data);

void free_rel_ts_fe(gpointer key, gpointer value,
                    gpointer user_data);

void synonym_f(TupleSet ts1, TupleSet ts2);
void inverseof_f(TupleSet ts1, TupleSet ts2);
void make_inter_relation(Manager man);
void make_reserved_relation(Manager man);

Manager init_manager() {
        Manager man = malloc(sizeof(struct manager));
        man->descriptions = makeDictionary(free);
        man->titles = makeDictionary(free);
        man->free_relations = makeDictionary(destroyTupleSet);

        make_inter_relation(man);
        make_reserved_relation(man);

        return man;
}

void apply_inter_relations(Manager man) {
        char buff[100];
        memcpy(buff, &(man->free_relations), sizeof(Dictionary));
        memcpy(buff + sizeof(Dictionary), &(man->relation_treatment), sizeof(Dictionary));
        printf("before foreach\n");
        foreachDictionary(man->inter_relations, treatment_foreach, buff);
        printf("after foreach\n");
}

void dump_manager(Manager man, char *dir) {
        printf("A iniciar despejo da informação na diretoria \"%s\".\n", dir);
        printf("---------------- Dados base:\n");
        printf("-> Existem %8d descrições.\n", sizeDictionary(man->descriptions));
        printf("-> Existem %8d titulos.\n", sizeDictionary(man->titles));
        printf("-> Existem %8d relações livres.\n",
               sizeDictionary(man->free_relations));

        // Processar todos os titulos e descricoes.

        // Processar todas as relações reservadas.

        // Processar todas as relações livres.
        foreachDictionary(man->free_relations, dump_free_rel, NULL);
}

void destroy_manager(Manager man) {
        destroyDictionary(man->descriptions);
        destroyDictionary(man->titles);
        destroyDictionary(man->free_relations);
        destroyDictionary(man->relation_treatment);
        destroyDictionary(man->inter_relations);
        destroyDictionary(man->reserved_relations);
}

int add_topic(Manager man, char *topic, char *title, char *description) {
        int res = 1;

        if(containsDictionary(man->descriptions, topic)
           || containsDictionary(man->titles, topic))
                return 0;

        // Insere o titulo
        insertDictionary(man->titles, topic, title);

        // Insere a descricao
        insertDictionary(man->descriptions, topic, description);

        return res;
}

int add_inter_relation(Manager man, char *relation, char *subject, char *object) {
        // Se relacao nao existe, abortar.
        if(!containsDictionary(man->inter_relations, relation))
                return -1;

        TupleSet ts = (TupleSet)getValueDictionary(man->inter_relations, relation);

        return insertTupleSet(ts, subject, object);
}

int add_relation(Manager man, char *relation, char *subject, char *object) {
        TupleSet ts = NULL;

        relation = strdup(relation);
        subject = strdup(subject);
        object = strdup(object);

        // se se trata de uma relacao reservada
        if( containsDictionary(man->reserved_relations, relation) ) {
                ts = (TupleSet)getValueDictionary(man->reserved_relations, relation);
        } else
                ts = (TupleSet)getValueDictionary(man->free_relations, relation);

        // Se nao existe
        if(!ts) {
                ts = makeTupleSet();
                insertDictionary(man->free_relations, relation, ts);
        }

        return insertTupleSet(ts, subject, object);
}

void synonym_foreach(gpointer key, gpointer value,
                     gpointer user_data) {
        char *key1, *key2;
        TupleSet ts = (TupleSet)user_data;
        char *tmp = get_keys(key, &key1, &key2);
        insertTupleSet(ts, key1, key2);
        free(tmp);
}

void inverseof_foreach(gpointer key, gpointer value,
                       gpointer user_data) {
        char *key1, *key2;
        TupleSet ts = (TupleSet)user_data;
        char *tmp = get_keys(key, &key1, &key2);
        insertTupleSet(ts, key2, key1);
        free(tmp);
}

void intreat_foreach(gpointer key, gpointer value,
                     gpointer user_data) {
        char *key1, *key2;
        Dictionary free_rel = *(Dictionary*)user_data;
        TreatFunc tf = *(TreatFunc*)(((char*)user_data) + sizeof(Dictionary));
        char *tmp = get_keys(key, &key1, &key2);
        printf("keys are %s and %s\n", key1, key2);
        TupleSet ts1 = (TupleSet)getValueDictionary(free_rel, key1);
        TupleSet ts2 = (TupleSet)getValueDictionary(free_rel, key2);
        printf("applying function %ld %ld\n", ts1, ts2);
        if(ts1 && ts2)
                (*tf)(ts1, ts2);
        printf("done applying stuff\n");
        free(tmp);
}

void treatment_foreach(gpointer key, gpointer value,
                       gpointer user_data) {
        char internalbuff[100];
        printf("relations is %s\n", (char*)key);
        printf("getting dictionary rel treat\n");
        Dictionary rel_treat = *(Dictionary*)(((char*)user_data) + sizeof(Dictionary));
        printf("getting treat func\n");
        TreatFunc tf = (TreatFunc)getValueDictionary(rel_treat, key);
        printf("copying memory\n");
        memcpy( internalbuff, (char*)user_data, sizeof(Dictionary) );
        memcpy( ((char*)internalbuff) + sizeof(Dictionary),
                &tf, sizeof(TreatFunc) );
        printf("before for each tuple\n");
        foreachTupleSet((TupleSet)value, intreat_foreach, internalbuff);
        printf("after for each tuple\n");
}

void synonym_f(TupleSet ts1, TupleSet ts2) {
        foreachTupleSet(ts1, synonym_foreach, ts2);
        foreachTupleSet(ts2, synonym_foreach, ts1);
}

void inverseof_f(TupleSet ts1, TupleSet ts2) {
        foreachTupleSet(ts1, inverseof_foreach, ts2);
        foreachTupleSet(ts2, inverseof_foreach, ts1);
}

void make_inter_relation(Manager man) {
        man->relation_treatment = makeDictionary(NULL);
        man->inter_relations = makeDictionary(destroyTupleSet);

        // Tratador do precicado synonym
        insertDictionary(man->relation_treatment,
                         g_strdup("synonym"), synonym_f);
        insertDictionary(man->inter_relations,
                         g_strdup("synonym"), makeTupleSet());

        // Tratador do predicado inverse_of
        insertDictionary(man->relation_treatment,
                         g_strdup("inverse_of"), inverseof_f);
        insertDictionary(man->inter_relations,
                         g_strdup("inverse_of"), makeTupleSet());
}

void make_reserved_relation(Manager man) {
        man->reserved_relations = makeDictionary(destroyTupleSet);

        insertDictionary(man->reserved_relations,
                         g_strdup("img"), makeTupleSet());
        insertDictionary(man->reserved_relations,
                         g_strdup("attach"), makeTupleSet());
}

void free_rel_ts_fe(gpointer key, gpointer value,
                    gpointer user_data) {
        char *key1, *key2;
        char *tmp = get_keys(key, &key1, &key2);

        printf("Contexto nº%d, sujeito \"%s\", objeto \"%s\".\n",
               contexto, key1, key2);
        contexto++;

        free(tmp);
}

void dump_free_rel(gpointer key, gpointer value,
                   gpointer user_data) {
        char *rel = (char*)key;
        TupleSet ts = (TupleSet)value;
        printf("\n---------------- Despejando info sobre \"%s\":\n", rel);
        printf("relação envolvida em %d contextos.\n", sizeTupleSet(ts));

        // Processar tupleset associado.
        contexto = 0;
        foreachTupleSet(ts, free_rel_ts_fe, NULL);
}
