#ifndef _TABLE_H
#define _TABLE_H

#include "cake.h"
#include "value.h"

typedef struct{
	ostr_t * key;
	value_t value;
} entry_t;

typedef struct{
	uint64_t count;
	uint64_t capacity;
	entry_t * entries;
} table_t;

table_t init_table(void);
bool get_table(table_t * table, ostr_t * key, value_t * value);
bool set_table(table_t * table, ostr_t * key, value_t value);
bool del_table(table_t * table, ostr_t * key);
void add2table(table_t * from, table_t * to);
ostr_t * table_find_str(table_t * table, const char * buffer, uint64_t length, uint32_t hash);
void free_table(table_t * table);

#endif
