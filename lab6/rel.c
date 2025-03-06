#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
	int first, second;
} Pair;

typedef struct {
	int size;
	Pair graph[MAX_REL_SIZE];
} Relation;

typedef struct {
	int size, values[MAX_REL_SIZE * 2];
} Set;

int pair_compare(const Pair *x, const Pair *y) {
	return x->first == y->first && x->second == y->second;
}

int pair_copy(Pair *dest, Pair *src) {
	dest->first = src->first;
	dest->second = src->second;
}

void relation_add(Relation *rel, Pair *p) {
	for (int i = 0; i < rel->size; i++) {
		if (pair_compare(&(rel->graph)[i], p)) return;
	}

	pair_copy(&(rel->graph[rel->size++]), p);
}

int relation_contains(Relation *rel, Pair *p) {
	for (int i = 0; i < rel->size; i++) {
		if (pair_compare(&(rel->graph[i]), p)) return 1;
	}

	return 0;
}

void relation_read(Relation *rel) {
	rel->size = 0;
	int n; Pair p;
	scanf("%d", &n);

	for (int i = 0; i < n; i++) {
		scanf("%d %d", &(p.first), &(p.second));
		relation_add(rel, &p);
	}
}

void set_add(Set *set, int value) {
	for (int i = 0; i < set->size; i++) {
		if (set->values[i] == value) return;
	}

	set->values[set->size++] = value;
}

int set_contains(Set *set, int value) {
	for (int i = 0; i < set->size; i++) {
		if (set->values[i] == value) return 1;
	}

	return 0;
}

void set_print(Set *set) {
	int temp;

	for (int i = 0; i < set->size; i++) {
		for (int j = 1; j < set->size - i; j++) {
			if (set->values[j - 1] > set->values[j]) {
				temp = set->values[j - 1];
				set->values[j - 1] = set->values[j];
				set->values[j] = temp;
			}
		}
	}

	printf("%d\n", set->size);
	for (int i = 0; i < set->size; i++) {
		printf("%d ", set->values[i]);
	}

	printf("\n");
}

// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(Relation *rel) {
	Pair p;

	for (int i = 0; i < rel->size; i++) {
		p.first = p.second = rel->graph[i].first;
		if (!relation_contains(rel, &p)) return 0;
	}

	return 1;
}

// The relation R on the set X is called irreflexive if xRx is false for every x in X
int is_irreflexive(Relation *rel) {
	Pair p;

	for (int i = 0; i < rel->size; i++) {
		p.first = p.second = rel->graph[i].first;
		if (relation_contains(rel, &p)) return 0;
	}

	return 1;
}

// A binary relation R over a set X is symmetric if for all x, y in X xRy <=> yRx
int is_symmetric(Relation *rel) {
	Pair p;

	for (int i = 0; i < rel->size; i++) {
		p.first = rel->graph[i].second;
		p.second = rel->graph[i].first;

		if (!relation_contains(rel, &p)) return 0;
	}

	return 1;
}

// A binary relation R over a set X is antisymmetric if for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(Relation *rel) {
	Pair p;

	for (int i = 0; i < rel->size; i++) {
		p.first = rel->graph[i].second;
		p.second = rel->graph[i].first;

		if (relation_contains(rel, &p) && p.first != p.second) return 0;
	}

	return 1;
}

// A binary relation R over a set X is asymmetric if for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(Relation *rel) {
	Pair p;

	for (int i = 0; i < rel->size; i++) {
		p.first = rel->graph[i].second;
		p.second = rel->graph[i].first;

		if (relation_contains(rel, &p)) return 0;
	}

	return 1;
}

// A homogeneous relation R on the set X is a transitive relation if for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(Relation *rel) {
	Pair p;
	for (int i = 0; i < rel->size; i++) {
		for (int j = 0; j < rel->size; j++) {
			if (rel->graph[i].second == rel->graph[j].first) {
				p.first = rel->graph[i].first;
				p.second = rel->graph[j].second;

				if (!relation_contains(rel, &p)) return 0;
			}
		}
	}

	return 1;
}

// Case 2:

// A partial order relation is a homogeneous relation that is reflexive, transitive, and antisymmetric
int is_partial_order(Relation *rel) {
	return is_reflexive(rel) && is_transitive(rel) && is_antisymmetric(rel);
}

// A relation R is connected if one of the following is true: xRy, yRx, x=y
int is_connected(Relation *rel) {
	Set s;
	s.size = 0;

	for (int i = 0; i < rel->size; i++) {
		set_add(&s, rel->graph[i].first);
		set_add(&s, rel->graph[i].second);
	}

	Pair p1, p2;
	for (int i = 0; i < s.size; i++) {
		for (int j = 0; j < s.size; j++) {
			if (s.values[i] == s.values[j]) continue;

			p1.first = p2.second = s.values[i];
			p1.second = p2.first = s.values[j];

			if (!(relation_contains(rel, &p1) || relation_contains(rel, &p2))) {
				return 0;
			}
		}
	}

	return 1;
}

// A total order relation is a partial order relation that is connected
int is_total_order(Relation *rel) {
	return is_partial_order(rel) && is_connected(rel);
}

void get_domain(Relation *rel, Set *result) {
	result->size = 0;

	for (int i = 0; i < rel->size; i++) {
		set_add(result, rel->graph[i].first);
		set_add(result, rel->graph[i].second);
	}
}

void find_max_elements(Relation *rel, Set *domain, Set *result) {
	int shouldAdd;
	result->size = 0;

	for (int i = 0; i < domain->size; i++) {
		shouldAdd = 1;

		for (int j = 0; j < rel->size; j++) {
			if (rel->graph[j].first == domain->values[i] && rel->graph[j].second != domain->values[i]) {
				shouldAdd = 0;
				break;
			}
		}

		if (shouldAdd) {
			set_add(result, domain->values[i]);
		}
	}
}

void find_min_elements(Relation *rel, Set *domain, Set *result) {
	int shouldAdd;
	result->size = 0;

	for (int i = 0; i < domain->size; i++) {
		shouldAdd = 1;

		for (int j = 0; j < rel->size; j++) {
			if (rel->graph[j].second == domain->values[i] && rel->graph[j].first != domain->values[i]) {
				shouldAdd = 0;
				break;
			}
		}

		if (shouldAdd) {
			set_add(result, domain->values[i]);
		}
	}
}

// Case 3:
int composition (Relation *rel1, Relation *rel2) {
	Relation result; Pair p;
	result.size = 0;

	for (int i = 0; i < rel1->size; i++) {
		for (int j = 0; j < rel2->size; j++) {
			if (rel1->graph[i].second == rel2->graph[j].first) {
				p.first = rel1->graph[i].first;
				p.second = rel2->graph[j].second;

				relation_add(&result, &p);
			}
		}
	}

	return result.size;
}

int main(void) {
	Relation rel, rel2;
	Set domain, max_elements, min_elements;

	int to_do, ordered;
	scanf("%d", &to_do);
	relation_read(&rel);

	switch (to_do) {
		case 1:
			printf(
				"%d %d %d %d %d %d\n",
				is_reflexive(&rel), is_irreflexive(&rel),
				is_symmetric(&rel), is_antisymmetric(&rel),
				is_asymmetric(&rel), is_transitive(&rel)
			);

			break;
		case 2:
			ordered = is_partial_order(&rel);
			get_domain(&rel, &domain);

			printf("%d %d\n", ordered, is_total_order(&rel));
			set_print(&domain);

			if (!ordered) break;

			find_max_elements(&rel, &domain, &max_elements);
			find_min_elements(&rel, &domain, &min_elements);

			set_print(&max_elements);
			set_print(&min_elements);

			break;
		case 3:
			relation_read(&rel2);
			printf("%d\n", composition(&rel, &rel2));

			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}