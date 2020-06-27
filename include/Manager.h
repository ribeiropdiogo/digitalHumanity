#ifndef _DIGI_MANAGER_
#define _DIGI_MANAGER_

#include "Dictionary.h"
#include "TupleSet.h"

typedef void (*TreatFunc)(TupleSet,TupleSet);

typedef struct manager *Manager;

Manager init_manager(char *dir);
void apply_inter_relations(Manager man);
void dump_manager(Manager man);
void destroy_manager(Manager man);
void add_reference(Manager man, char *topic);
void add_title(Manager man, char *topic, char *title);
void add_description(Manager man, char *topic, char *description);
int add_inter_relation(Manager man, char *relation, char *subject, char *object);
int add_relation(Manager man, char *relation, char *subject, char *object);

#endif
