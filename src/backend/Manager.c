#include <stdio.h>
#include "Manager.h"
#include "html_converter.h"

int contexto;
char *maindir;

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

        Dictionary reference;

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

void foreach_escrita_file(gpointer key, gpointer value,
                          gpointer user_data);

void synonym_f(TupleSet ts1, TupleSet ts2);
void inverseof_f(TupleSet ts1, TupleSet ts2);
void make_inter_relation(Manager man);
void make_reserved_relation(Manager man);
char *make_side_bar(Dictionary dict);

Dictionary getAllCodeInfo(Manager man);
void processa_titulos(Dictionary dict, Manager man);
void processa_imagens(Dictionary dict, Manager man);
void processa_descricao(Dictionary dict, Manager man);
void processa_relacoes(Dictionary dict, Manager man);
void processa_anexos(Dictionary dict, Manager man);
void dict_append_info(Dictionary dict, char *topic,
                      char *format);
void escrever_ficheiro(char *nome, char *sidebar, char *info);
void dump_file(Manager man, char *dir);

Manager init_manager() {
        Manager man = malloc(sizeof(struct manager));
        man->descriptions = makeDictionary(free);
        man->titles = makeDictionary(free);
        man->free_relations = makeDictionary(destroyTupleSet);
        man->reference = makeDictionary(free);

        make_inter_relation(man);
        make_reserved_relation(man);

        return man;
}

void apply_inter_relations(Manager man) {
        char buff[100];
        memcpy(buff, &(man->free_relations), sizeof(Dictionary));
        memcpy(buff + sizeof(Dictionary), &(man->relation_treatment), sizeof(Dictionary));
        foreachDictionary(man->inter_relations, treatment_foreach, buff);
}

void destroy_manager(Manager man) {
        destroyDictionary(man->descriptions);
        destroyDictionary(man->titles);
        destroyDictionary(man->reference);
        destroyDictionary(man->free_relations);
        destroyDictionary(man->relation_treatment);
        destroyDictionary(man->inter_relations);
        destroyDictionary(man->reserved_relations);
}

void add_reference(Manager man, char *topic) {
        insertDictionary(man->reference, topic, NULL);
}

void add_title(Manager man, char *topic, char *title) {
        insertDictionary(man->titles, topic, title);
}

void add_description(Manager man, char *topic, char *description) {
        insertDictionary(man->descriptions, topic, description);
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

        TupleSet ts1 = (TupleSet)getValueDictionary(free_rel, key1);
        TupleSet ts2 = (TupleSet)getValueDictionary(free_rel, key2);

        if(ts1 && ts2)
                (*tf)(ts1, ts2);

        free(tmp);
}

void treatment_foreach(gpointer key, gpointer value,
                       gpointer user_data) {
        char internalbuff[100];

        Dictionary rel_treat = *(Dictionary*)(((char*)user_data) + sizeof(Dictionary));

        TreatFunc tf = (TreatFunc)getValueDictionary(rel_treat, key);

        memcpy( internalbuff, (char*)user_data, sizeof(Dictionary) );
        memcpy( ((char*)internalbuff) + sizeof(Dictionary),
                &tf, sizeof(TreatFunc) );

        foreachTupleSet((TupleSet)value, intreat_foreach, internalbuff);
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

//        printf("Contexto nº%d, sujeito \"%s\", objeto \"%s\".\n", contexto, key1, key2);

        contexto++;

        free(tmp);
}

void dump_free_rel(gpointer key, gpointer value,
                   gpointer user_data) {
        char *rel = (char*)key;
        TupleSet ts = (TupleSet)value;
//        printf("\n---------------- Despejando info sobre \"%s\":\n", rel);
//        printf("relação envolvida em %d contextos.\n", sizeTupleSet(ts));

        // Processar tupleset associado.
        contexto = 0;
        foreachTupleSet(ts, free_rel_ts_fe, NULL);
}


void dump_manager(Manager man, char *dir) {
        char folder[1000];
        sprintf(folder, "cp -r ../html-template %s", dir);
        int x = system(folder);
        printf("A iniciar despejo da informação na diretoria \"%s\".\n", dir);
        printf("---------------- Dados base:\n");
        printf("-> Existem %8d descrições.\n", sizeDictionary(man->descriptions));
        printf("-> Existem %8d titulos.\n", sizeDictionary(man->titles));
        printf("-> Existem %8d relações livres.\n",
               sizeDictionary(man->free_relations));

        maindir = dir;
        Dictionary dict = makeDictionary(free);
        char *sidebar, nm[1000];
        sprintf(nm, "%s/index.html", maindir);

        processa_titulos(dict, man);
        processa_imagens(dict, man);
        processa_descricao(dict, man);
        processa_relacoes(dict, man);
        processa_anexos(dict, man);

        sidebar = make_side_bar(dict);

        printf("coisas\n");

        escrever_ficheiro(nm, sidebar, NULL);

        printf("after\n");

        foreachDictionary(dict,
                          foreach_escrita_file, sidebar);

        destroyDictionary(dict);
}

void processa_titulos(Dictionary dict, Manager man) {

}

void processa_imagens(Dictionary dict, Manager man) {

}

void processa_descricao(Dictionary dict, Manager man) {

}

void processa_relacoes(Dictionary dict, Manager man) {

}

void processa_anexos(Dictionary dict, Manager man) {

}

void dict_append_info(Dictionary dict, char *topic,
                      char *format) {
        GString *buff = (GString*)getValueDictionary(dict, topic);
        int empty = !buff;

        if( empty ) {
                buff = g_string_new(NULL);
        }

        g_string_append(buff, format);

        if(empty) {
                insertDictionary(dict, topic, buff);
        }
}

void escrever_ficheiro(char *nome, char *sidebar, char *info) {
        FILE *fp = fopen(nome, "w+");

        printf("before files\n");

        char *head = file_heading(),
             *middle = file_middle(),
             *minfo = info ? info : index_intro(),
             *end = file_ending();

        printf("after files\n");

        printf("depois do ola\n");
        fprintf(fp, "%s%s%s%s%s", head,
                sidebar, middle, minfo, end);

        printf("after write");

        fclose(fp);

        free(head);
        free(sidebar);
        free(middle);
        free(minfo);
        free(end);
}

char *make_side_bar(Dictionary dict) {
        return strdup("");
}

void foreach_escrita_file(gpointer key, gpointer value,
                          gpointer user_data) {
        char nome[1000], *sidebar = (char*)user_data;
        sprintf(nome, "%s/files/%s.html",
                maindir, (char*)key);
        escrever_ficheiro(nome, sidebar, (char*)value);
}
