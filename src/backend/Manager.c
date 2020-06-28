#include <stdio.h>
#include "Manager.h"
#include "html_converter.h"

int contexto;
char *maindir;
Dictionary tmp;

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

void foreach_add_to_arr(gpointer key, gpointer value,
                        gpointer user_data);

void add_titus_fe(gpointer key, gpointer value,
                  gpointer user_data);

void add_desks_fe(gpointer key, gpointer value,
                  gpointer user_data);

void fe_images(gpointer key, gpointer value,
               gpointer user_data);

void fe_add_images(gpointer key, gpointer value,
                   gpointer user_data);

void fe_add_anexs(gpointer key, gpointer value,
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

Manager init_manager(char *dir) {
        char folder[1000];
        maindir = dir;

        sprintf(folder, "cp -r /usr/local/lib/html-template %s", maindir);
        int x = system(folder);

        maindir = dir;
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
        insertDictionary(man->reference, strdup(topic), g_string_new(NULL));
        insertDictionary(man->descriptions, topic, description);
}

int add_inter_relation(Manager man, char *relation, char *subject, char *object) {
        // Se relacao nao existe, abortar.
        if(!containsDictionary(man->inter_relations, relation))
                return -1;

        TupleSet ts = (TupleSet)getValueDictionary(man->inter_relations, relation);

        if( !containsDictionary(man->free_relations, subject) )
                insertDictionary(man->free_relations, subject,
                                 makeTupleSet());

        if( !containsDictionary(man->free_relations, object) )
                insertDictionary(man->free_relations, object,
                                 makeTupleSet());

        return insertTupleSet(ts, subject, object);
}

int add_relation(Manager man, char *relation, char *subject, char *object) {
        char buff[1000];
        TupleSet ts = NULL;

        relation = strdup(relation);
        subject = strdup(subject);
        object = strdup(object);

        // se se trata de uma relacao reservada
        if( containsDictionary(man->reserved_relations, relation) ) {
                ts = (TupleSet)getValueDictionary(man->reserved_relations, relation);
                if( !strcmp(relation,"img") ) {
                        sprintf(buff, "cp %s %s/images/", object, maindir);
                }
                else {
                        sprintf(buff, "cp %s %s/", object, maindir);
                }
                int x = system(buff);
        } else {
                ts = (TupleSet)getValueDictionary(man->free_relations, relation);
                insertDictionary(man->reference, strdup(subject), g_string_new(NULL));
                insertDictionary(man->reference, strdup(object), g_string_new(NULL));
        }

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
        char *sujeito, *objeto,buff[2000];
        char *keys = get_keys(key, &sujeito, &objeto);
        char *relacao = (char*)user_data;
        Dictionary dict = (Dictionary)user_data;

        printf("Contexto nº%d, sujeito \"%s\", objeto \"%s\".\n",
               contexto, sujeito, objeto);

        sprintf(buff, "<li><a href=\"%s.html\">%s</a> %s <a href=\"%s.html\">%s</a></li>\n",
                sujeito, sujeito, relacao, objeto, objeto);

        dict_append_info(tmp, sujeito, buff);
        dict_append_info(tmp, objeto, buff);

        contexto++;

        free(keys);
}

void dump_free_rel(gpointer key, gpointer value,
                   gpointer user_data) {
        char *rel = (char*)key;
        TupleSet ts = (TupleSet)value;
        printf("\n---------------- Despejando info sobre \"%s\":\n", rel);
        printf("relação envolvida em %d contextos.\n", sizeTupleSet(ts));

        // Processar tupleset associado.
        contexto = 0;
        foreachTupleSet(ts, free_rel_ts_fe, key);
}


void dump_manager(Manager man) {
        printf("A iniciar despejo da informação na diretoria \"%s\".\n", maindir);
        printf("---------------- Dados base:\n");
        printf("-> Existem %8d descrições.\n", sizeDictionary(man->descriptions));
        printf("-> Existem %8d titulos.\n", sizeDictionary(man->titles));
        printf("-> Existem %8d relações livres.\n",
               sizeDictionary(man->free_relations));

        char *sidebar, nm[1000];
        sprintf(nm, "%s/index.html", maindir);

        processa_titulos(man->reference, man);
        processa_imagens(man->reference, man);
        processa_descricao(man->reference, man);
        processa_relacoes(man->reference, man);
        processa_anexos(man->reference, man);

        sidebar = make_side_bar(man->reference);

        escrever_ficheiro(nm, sidebar, NULL);
        foreachDictionary(man->reference,
                          foreach_escrita_file, sidebar);
}

void processa_titulos(Dictionary dict, Manager man) {
        tmp = man->titles;
        foreachDictionary(dict, add_titus_fe, NULL);
}

void processa_imagens(Dictionary dict, Manager man) {
        tmp = dict;
        Dictionary imgs = makeDictionary(NULL);
        TupleSet ts = getValueDictionary(man->reserved_relations, "img");
        foreachTupleSet(ts, fe_images, imgs);
        foreachDictionary(imgs, fe_add_images, NULL);

        destroyDictionary(imgs);
}

void processa_descricao(Dictionary dict, Manager man) {
        tmp = man->descriptions;
        foreachDictionary(dict, add_desks_fe, NULL);
}

void processa_relacoes(Dictionary dict, Manager man) {
        tmp = dict;
        foreachDictionary(man->free_relations, dump_free_rel, NULL);
}

void processa_anexos(Dictionary dict, Manager man) {
        tmp = dict;
        Dictionary anexs = makeDictionary(NULL);
        TupleSet ts = getValueDictionary(man->reserved_relations, "attach");
        foreachTupleSet(ts, fe_images, anexs);
        foreachDictionary(anexs, fe_add_anexs, NULL);

        destroyDictionary(anexs);
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

        char *head = file_heading(),
             *middle = file_middle(),
             *minfo = info ? info : index_intro(),
             *end = file_ending();

        fprintf(fp, "%s%s%s%s%s", head,
                sidebar, middle, minfo, end);

        fclose(fp);

        free(head);
        free(middle);
        free(minfo);
        free(end);
}

char *make_side_bar(Dictionary dict) {
        GArray *arr = g_array_new(FALSE, FALSE, sizeof(char*));

        foreachDictionary(dict, foreach_add_to_arr, arr);

        char *sidebar = sidebar_info(arr);

        g_array_free(arr, TRUE);

        return sidebar;
}

void foreach_escrita_file(gpointer key, gpointer value,
                          gpointer user_data) {
        char nome[1000], *sidebar = (char*)user_data;
        sprintf(nome, "%s/%s.html",
                maindir, (char*)key);

        escrever_ficheiro(nome, sidebar, g_string_free((GString*)value, FALSE));
}

void foreach_add_to_arr(gpointer key, gpointer value,
                        gpointer user_data) {
        char *name = strdup((char*)key);
        g_array_append_val((GArray*)user_data, name);
}


void add_titus_fe(gpointer key, gpointer value,
                  gpointer user_data) {
        char *tit = (char*)key;
        GString *buff = (GString*)value;
        char *title = adiciona_titulo(containsDictionary(tmp,tit) ? (char*)getValueDictionary(tmp,tit) : tit);
        g_string_append(buff, title);
        free(title);
}

void fe_add_images(gpointer key, gpointer value,
                   gpointer user_data) {
        char *tit = (char*)key;
        GArray *arr = (GArray*)value;

        char *imginfo = image_displayer(arr);

        dict_append_info(tmp, tit, imginfo);
}

void add_desks_fe(gpointer key, gpointer value,
                  gpointer user_data) {
        char *tit = (char*)key;
        GString *buff = (GString*)value;
        if( containsDictionary(tmp, tit) )
                g_string_append(buff, (char*)getValueDictionary(tmp, tit));
        g_string_append_printf(buff, "<h2 class=\"section__title\">Relações</h2>\n");
}

void fe_images(gpointer key, gpointer value,
               gpointer user_data) {
        char *sujeito, *objeto,buff[2000];
        char *keys = get_keys(key, &sujeito, &objeto),
             *cc = strdup(objeto), *suj = strdup(sujeito);
        Dictionary imgs = (Dictionary)user_data;

        GArray *arr = (GArray*)getValueDictionary(imgs, suj);

        if(!arr)
                arr = g_array_new(FALSE, FALSE, sizeof(char*));

        g_array_append_val(arr, cc);

        if(!containsDictionary(imgs, suj))
                insertDictionary(imgs, suj, arr);

        free(keys);
}

void fe_add_anexs(gpointer key, gpointer value,
                  gpointer user_data) {
        char *tit = (char*)key;
        GArray *arr = (GArray*)value;

        char *annexinfo = add_attachments(arr);

        dict_append_info(tmp, tit, annexinfo);
}
