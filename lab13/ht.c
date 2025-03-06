#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR (-1)

typedef union {
	int int_data;
	char char_data;
	void *ptr_data;
} DataUnion;

typedef struct HashTableElement {
	struct HashTableElement *next;
	DataUnion data;
} HashTableElement;

typedef void (*DataFp)(DataUnion);
typedef void (*DataPFp)(DataUnion*);
typedef int (*CompareDataFp)(DataUnion, DataUnion);
typedef size_t (*HashFp)(DataUnion, size_t);
typedef DataUnion (*CreateDataFp)(void*);

typedef struct {
	size_t size, no_elements;
	HashTableElement *ht;
	DataFp dump_data, free_data;
	DataPFp modify_data;
	CreateDataFp create_data;
	CompareDataFp compare_data;
	HashFp hash_function;
} HashTable;

// ---------------------- functions to implement

// initialize table fields
void init_ht(
	HashTable *table, size_t size, DataFp dump_data,
	CreateDataFp create_data, DataFp free_data, CompareDataFp compare_data,
	HashFp hash_function, DataPFp modify_data
) {
	table->ht = malloc(size * sizeof(HashTableElement));
 	table->size = size;
	table->no_elements = 0;
	table->dump_data = dump_data;
	table->free_data = free_data;
	table->modify_data = modify_data;
	table->create_data = create_data;
	table->compare_data = compare_data;
	table->hash_function = hash_function;

	for (size_t i = 0; i < size; i++) {
		table->ht[i].next = NULL;
	}
}

// print elements of the list with hash n
void dump_list(const HashTable *table, size_t n) {
	HashTableElement *element = table->ht[n].next;

	while (element != NULL) {
		table->dump_data(element->data);
		element = element->next;
	}

	printf("\n");
}

// Free element pointed by DataUnion using free_data() function
void free_element(DataFp free_data, HashTableElement *to_delete) {
	HashTableElement *node;

	while (to_delete != NULL) {
		if (free_data) {
			free_data(to_delete->data);
		}

		to_delete = to_delete->next;
	}
}

// free all elements from the table (and the table itself)
void free_table(HashTable *table) {
	for (size_t i = 0; i < table->size; i++) {
		free_element(table->free_data, table->ht + i);
	}

	free(table->ht);
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
	static const double c = 0.618033988; // (sqrt(5.) â 1) / 2.;
	double tmp = k * c;
	return (size_t)floor((double)size * (tmp - floor(tmp)));
}

void insertToList(HashTableElement *first, DataUnion *data) {
	HashTableElement *node = malloc(sizeof(HashTableElement));
	node->data = *data;
	node->next = first->next;
	first->next = node;
}

void rehash(HashTable *table) {
	HashTableElement *node, *newTable = malloc(table->size * 2 * sizeof(HashTableElement));
	for (size_t i = 0; i < table->size * 2; i++) {
		newTable[i].next = NULL;
	}

	for (size_t i = 0; i < table->size; i++) {
		node = table->ht[i].next;

		while (node != NULL) {
			insertToList(
				&newTable[table->hash_function(node->data, table->size * 2)],
				&node->data
			);

			node = node->next;
		}
	}

	table->size *= 2;
	table->ht = newTable;
}

// find element; return pointer to previous
HashTableElement *find_previous(HashTable *table, DataUnion *data) {
	HashTableElement *node = &table->ht[table->hash_function(*data, table->size)];

	while (node->next != NULL && table->compare_data(node->next->data, *data)) {
		node = node->next;
	}

	return node;
}

// return pointer to element with given value
HashTableElement *get_element(HashTable *table, DataUnion *data) {
	HashTableElement *node = table->ht[table->hash_function(*data, table->size)].next;

	while (node != NULL && table->compare_data(node->data, *data)) {
		node = node->next;
	}

	return node;
}

// insert element
void insert_element(HashTable *table, DataUnion *data) {
	HashTableElement *node = get_element(table, data);

	if (node == NULL) {
		insertToList(&table->ht[table->hash_function(*data, table->size)], data);
		table->no_elements++;

		if ((float)table->no_elements / table->size > MAX_RATE) {
			rehash(table);
		}
	} else if (table->modify_data != NULL) {
		table->modify_data(&node->data);
	}
}

// remove element
void remove_element(HashTable *table, DataUnion data) {
	HashTableElement *node = find_previous(table, &data), *next;

	if (node->next != NULL) {
		next = node->next->next;
		free(node->next);
		node->next = next;
	}
}

// type-specific definitions

// int element

size_t hash_int(DataUnion data, size_t size) {
	return hash_base(data.int_data, size);
}

void dump_int(DataUnion data) {
	printf("%d ", data.int_data);
}

int cmp_int(DataUnion a, DataUnion b) {
	return a.int_data - b.int_data;
}

DataUnion create_int(void *value) {
	DataUnion result;
	scanf("%d", &result.int_data);

	return result;
}

// char element

size_t hash_char(DataUnion data, size_t size) {
	return hash_base((int)data.char_data, size);
}

void dump_char(DataUnion data) {
	printf("%c ", data.char_data);
}

int cmp_char(DataUnion a, DataUnion b) {
	return a.char_data - b.char_data;
}

DataUnion create_char(void *value) {
	DataUnion result;
	scanf(" %c", &result.char_data);

	return result;
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word(DataUnion data) {
	DataWord *d = (DataWord *)data.ptr_data;
	printf("%s %d\n", d->word, d->counter);
}

void free_word(DataUnion data) {
	DataWord *d = (DataWord *)data.ptr_data;
	free(data.ptr_data);
}

int cmp_word(DataUnion a, DataUnion b) {
	return strcmp(((DataWord *)a.ptr_data)->word, ((DataWord *)b.ptr_data)->word);
}

size_t hash_word(DataUnion data, size_t size) {
	int s = 0;
	DataWord *dw = (DataWord*)data.ptr_data;
	char *p = dw->word;
	while (*p) {
		s += *p++;
	}
	return hash_base(s, size);
}

void modify_word(DataUnion *data) {
	DataWord *word = (DataWord *)data->ptr_data;
	(word->counter)++;
}

DataUnion create_data_word(void *value) {
	char *word = (char *)value;

	DataWord *data = malloc(sizeof(DataWord));
	data->counter = 1;
	data->word = strdup(value);

	DataUnion result = {
		.ptr_data = data
	};

	return result;
}

// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(HashTable *table, FILE *stream) {
	char buffer[BUFFER_SIZE], *delimiters = " \n\t\r\v\f.,?!:;-";
	DataUnion data;

	while (!feof(stream)) {
		buffer[0] = '\0';
		fgets(buffer, BUFFER_SIZE, stream);

		for (
			char *word = strtok(buffer, delimiters);
			word != NULL;
			word = strtok(NULL, delimiters)
		) {
			if (strlen(word)) {
				for (size_t i = 0; word[i] != '\0'; i++) {
					word[i] = tolower(word[i]);
				}

				data = table->create_data(word);
				insert_element(table, &data);
			}
		}
	}
}

// test primitive type list
void test_ht(HashTable *p_table, int n) {
	char op;
	DataUnion data;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
		data = p_table->create_data(NULL);

		switch (op) {
			case 'r':
				remove_element(p_table, data);
				break;
			case 'i':
				insert_element(p_table, &data);
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

int main(void) {
	int to_do, n;
	size_t index;
	HashTable table;
	char buffer[BUFFER_SIZE];
	DataUnion data;

	scanf("%d", &to_do);
	switch (to_do) {
		case 1: // test integer hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 2: // test char hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 3: // read words from text, insert into hash table, and print
			scanf("%s", buffer);
			init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
			stream_to_ht(&table, stdin);
			printf ("%zu\n", table.size);
			data = table.create_data(buffer);
			HashTableElement *e = get_element(&table, &data);
			if (e) table.dump_data(e->data);
			if (table.free_data) table.free_data(data);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	free_table(&table);

	return 0;
}