#ifndef _DIGI_MANAGER_
#define _DIGI_MANAGER_

#include "Dictionary.h"
#include "TupleSet.h"

typedef void (*TreatFunc)(TupleSet,TupleSet);

typedef struct manager *Manager;

Manager init_manager();
void apply_inter_relations(Manager man);
void destroy_manager(Manager man);
int add_topic(Manager man, char *topic, char *title, char *description);
int add_inter_relation(Manager man, char *relation, char *subject, char *object);
int add_relation(Manager man, char *relation, char *subject, char *object);

#endif
