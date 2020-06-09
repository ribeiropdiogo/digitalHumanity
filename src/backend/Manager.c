#include "Manager.h"

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
        Dicionary titles;

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
        Dicionary free_relations;

        /*
           Associa a cada relação o conjunto de tuplos
           que tiram partido dessa.
         */
        Dicionary reserved_relations;

        /*
           Associa a cada relação inter-relações o conjunto
           de tuplos que tiram partido desta.
         */
        Dicionary inter_relations;
} *Manager;

int add_topic(Manager man, char *topic, char *title, char *description) {
        int res = 1;

        if(containsDictionary(descriptions, topic)
           || containsDictionary(titles, topic))
                return 0;

        // Insere o titulo
        insertDictionary(titles, topic, title);

        // Insere a descricao
        insertDictionary(descriptions, topic, description);

        return res;
}

int add_inter_relation(Manager man, char *relation, char *subject, char *object) {
        // Se relacao nao existe, abortar.
        if(!containsDictionary(inter_relations, relation))
                return 0;

        TupleSet ts = (TupleSet)getValueDictionary(inter_relations, relation);

        return insertTupleSet(ts, subject, object);
}

int add_relation(Manager man, char *relation, char *subject, char *object) {
        TupleSet ts = NULL;

        // se se trata de uma relacao reservada
        if( containsDictionary(reserved_relations, relation) ) {
                ts = (TupleSet)getValueDictionary(reserved_relations, relation);
        } else
                ts = (TupleSet)getValueDictionary(free_relations, relation);

        // Se nao existe
        if(!ts) {
                ts = makeTupleSet();
                insertDictionary(free_relations, relation, ts);
        }

        return insertTupleSet(ts, subject, object);
}
