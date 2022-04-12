#include "../include/table.h"
#include "../include/memory.h"
#include "../include/object.h"

/* When table is 75% full, grow the array */
#define TABLE_MAX_LOAD (0.75)

table_t init_table(void){
	table_t table = {
		.count = 0,
		.capacity = 0,
		.entries = NULL,
	};
	return table;
}

static entry_t * find_entry(entry_t * entries, uint64_t capacity, ostr_t * key){
	uint32_t index = key->hash % capacity;
	entry_t * tombstone = NULL;
	while (1){
		entry_t * entry = &entries[index];
		if (entry->key == NULL){
			if (IS_NIL(entry->value)){
				/* empty entry */
				return tombstone != NULL ? tombstone : entry;
			}else{
				/* tombstone */
				if (tombstone == NULL){
					tombstone = entry;
				}
			}
		}else if (entry->key == key){
			/* found key */
			return entry;
		}
		index = (index + 1) & (capacity - 1);
	}
}

static void adjust_capacity(table_t * table, uint64_t capacity){
	entry_t * entries = ALLOCATE(entry_t, capacity);
	for (uint64_t i = 0; i < capacity; ++i){
		entries[i].key = NULL;
		entries[i].value = NIL_VAL;
	}

	table->count = 0;
	for (uint64_t i = 0; i < table->capacity; ++i){
		entry_t * entry = &table->entries[i];
		if (entry->key == NULL){
			continue;
		}

		entry_t * dest = find_entry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
		++table->count;
	}

	FREE_ARRAY(entry_t, table->entries, table->capacity);
	table->entries = entries;
	table->capacity = capacity;
}

bool get_table(table_t * table, ostr_t * key, value_t * value){
	if (!table->count){
		return false;
	}

	entry_t * entry = find_entry(table->entries, table->capacity, key);
	if (entry->key == NULL){
		return false;
	}

	*value = entry->value;
	return true;
}

bool set_table(table_t * table, ostr_t * key, value_t value){
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD){
		uint64_t capacity = GROW_CAPACITY(table->capacity);
		adjust_capacity(table, capacity);
	}
	entry_t * entry = find_entry(table->entries, table->capacity, key);
	bool new_key = entry->key == NULL;

	if (new_key && IS_NIL(entry->value)){
		++table->count;
	}

	entry->key = key;
	entry->value = value;
	return new_key;
}

bool del_table(table_t * table, ostr_t * key){
	if (!table->count){
		return false;
	}

	entry_t * entry = find_entry(table->entries, table->capacity, key);
	if (entry->key == NULL){
		return false;
	}

	/* place tombstone */
	entry->key = NULL;
	entry->value = BOOL_VAL(true);
	return true;
}

void add2table(table_t * from, table_t * to){
	for (uint64_t i = 0; i < from->capacity; ++i){
		entry_t * entry = &from->entries[i];
		if (entry->key != NULL){
			set_table(to, entry->key, entry->value);
		}
	}
}

ostr_t * table_find_str(table_t * table, const char * buffer, uint64_t length, uint32_t hash){
	if (!table->count){
		return NULL;
	}

	uint32_t index = hash % table->capacity;
	while (1){
		entry_t * entry = &table->entries[index];

		if (entry->key == NULL){
			/* empty non-tombstone entry */
			if (IS_NIL(entry->value)){
				return NULL;
			}
		}else if (entry->key->length == length && entry->key->hash == hash &&\
				memcmp(entry->key->buffer, buffer, length) == 0){
			/* found key */
			return entry->key;
		}

		index = (index + 1) % table->capacity;
	}
}

void free_table(table_t * table){
	FREE_ARRAY(entry_t, table->entries, table->capacity);
	*table = init_table();
}
