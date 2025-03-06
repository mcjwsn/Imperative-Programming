#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define FOOD_MAX 30
#define ART_MAX 15
#define RECORD_MAX 40
#define CHILD_MAX 20

typedef int (*ComparFp)(const void *, const void *);

typedef struct {
	int day, month, year;
} Date;

typedef struct {
	char name[20];
	int amount;
	float price;
	Date valid_date;
} Food;

typedef struct {
	char *name, *parent;
	unsigned char gender, in_line;
	Date born;
} Person;

typedef struct {
	char *par_name;
	int index;
} Parent;

const Date primo_date = { 28, 10, 2011 };

enum Sex {F, M};
enum BOOL {no, yes};

int cmp_date(const void *d1, const void *d2) {
	const Date *date1 = (Date *)d1, *date2 = (Date *)d2;

	return date1->year == date2->year ? (
		date1->month == date2->month ? (date2->day - date1->day) : (date2->month - date1->month)
	) : (date2->year - date1->year);
}

int cmp_food(const void *a, const void *b) {
	const Food *food1 = (Food *)a, *food2 = (Food *)b;

	if (strcmp(food2->name, food1->name)) {
		return strcmp(food2->name, food1->name);
	}

	if (food1->price != food2->price) {
		return food2->price > food1->price ? 1 : -1;
	}

	if (cmp_date(&food1->valid_date, &food2->valid_date)) {
		return cmp_date(&food1->valid_date, &food2->valid_date);
	}

	return 0;
}

int cmp_qs(const void *a, const void *b) {
	Food *fa = (Food*)a, *fb = (Food*)b;
	return cmp_date(&fa->valid_date, &fb->valid_date);
}

void *bsearch2(const void *key, const void *base, size_t count, size_t size, ComparFp compar, char *result) {
	int cmp, left = 0, right = count, mid;

	while (left < right) {
		mid = left + (right - left) / 2;
		cmp = (*compar)(key, base + mid * size);

		if (cmp == 0) {
			left = mid;
			break;
		} else if (cmp < 0) {
			left = mid + 1;
		} else {
			right = mid;
		}
	}

	if (count) {
		*result = !(*compar)(key, base + left * size);
	}

	return (void *) (base + left * size);
}

void print_art(Food *p, int n, char *art) {
	for (int i = 0; i < n; i++) {
		if (art == NULL || strcmp(art, (p + i)->name) == 0) {
			if (art == NULL) {
				printf("%s ", (p + i)->name);
			}
			printf("%.2f %d ", (p + i)->price, (p + i)->amount);
			printf("%.2d.%.2d.%d\n", (p + i)->valid_date.day, (p + i)->valid_date.month, (p + i)->valid_date.year);
		}
	}
}

Food *add_record(Food *tab, int *count, ComparFp compar, Food *new, int sorted) {
	if (!sorted) {
		tab[(*count)++] = *new;
		return tab + *count - 1;
	}

	char found = 0;
	Food *foundFood = (Food *)bsearch2(new, tab, *count, sizeof(Food), compar, &found);

	int i;
	if (found) {
		foundFood->amount += new->amount;
	} else {
		i = (*count)++;
		while (tab + i != foundFood) {
			tab[i] = tab[i - 1];
			i++;
		}

		tab[i] = *new;
	}

	return foundFood;
}

int read_goods(Food *tab, int no, FILE *stream, int sorted) {
	Food new_food;
	char found;
	int size = 0;

	for (int i = 0; i < no; i++) {
		fscanf(
			stream, "%s %f %d %d.%d.%d",
			new_food.name, &new_food.price, &new_food.amount,
			&new_food.valid_date.day, &new_food.valid_date.month, &new_food.valid_date.year
		);

		add_record(tab, &size, cmp_food, &new_food, sorted);
	}

	return size;
}

Food *lowerBoundFood(Food *tab, size_t n, Date *date) {
	size_t left = 0, right = n, mid;

	while (left < right) {
		mid = left + (right - left) / 2;

		if (cmp_date(&(tab + mid)->valid_date, date) < 0) {
			left = mid + 1;
		} else {
			right = mid;
		}
	}

	return tab + left;
}

void addDaysToDate(Date *date, int days) {
	struct tm t = {
		.tm_year = date->year - 1900,
		.tm_mon = date->month - 1,
		.tm_mday = date->day + days
	};

	mktime(&t);

	date->day = t.tm_mday;
	date->month = t.tm_mon + 1;
	date->year = t.tm_year + 1900;
}

float value(Food *food_tab, size_t n, Date curr_date, int days) {
	float result = 0;
	qsort(food_tab, n, sizeof(Food), cmp_qs);

	addDaysToDate(&curr_date, days);
	for (
		Food *i = lowerBoundFood(food_tab, n, &curr_date);
		!cmp_date(&i->valid_date, &curr_date) && i < food_tab + n;
		i++
	) {
		result += i->amount * i->price;
	}

	return result;
}

int cmp_person(const void *a, const void *b) {
	Person *p1 = (Person *)a, *p2 = (Person *)b;

	if (p1->parent == NULL || p2->parent == NULL) {
		return p2->parent == NULL ? 1 : -1;
	}

	if (strcmp(p1->parent, p2->parent)) {
		return strcmp(p1->parent, p2->parent);
	}

	if (p1->gender != p2->gender && (cmp_date(&p1->born, &primo_date) >= 0 || cmp_date(&p2->born, &primo_date) >= 0)) {
		return p2->gender - p1->gender;
	}

	return cmp_date(&p2->born, &p1->born);
}

int childrenBounds(Person *tab, int *n, int *index, int bounds[2]) {
	char *parentName = tab[*index].name;

	bounds[0] = *index + 1;
	while (bounds[0] < *n && strcmp(parentName, tab[bounds[0]].parent)) bounds[0]++;

	bounds[1] = bounds[0];
	while (bounds[1] < *n && !strcmp(parentName, tab[bounds[1]].parent)) bounds[1]++;

	return bounds[0] <= --bounds[1] && bounds[1] < *n;
}

void moveBlockOfPeople(Person *person_tab, int n, int index) {
	int bounds[2];

	if (!childrenBounds(person_tab, &n, &index, bounds)) return;

	Person temp;
	while (--bounds[0] > index) {
		temp = person_tab[bounds[0]];

		for (int i = bounds[0] + 1; i <= bounds[1]; i++) {
			person_tab[i - 1] = person_tab[i];
		}

		person_tab[bounds[1]--] = temp;
	}
}

void create_list(Person *person_tab, int n) {
	qsort(person_tab, n, sizeof(Person), cmp_person);

	for (int i = 0; i < n; i++) {
		moveBlockOfPeople(person_tab, n, i);
	}

	int personCounter = 0;
	for (int i = 0; i < n; i++) {
		if (person_tab[personCounter].in_line == no) {
			for (int j = personCounter + 1; j < n; j++) {
				person_tab[j - 1] = person_tab[j];
			}
		} else {
			personCounter++;
		}
	}
}

int main(void) {
	Person person_tab[] = {
		{"Charles III", "Elizabeth II", M, no, {14, 11, 1948}},
		{"Anne", "Elizabeth II", F, yes, {15, 8, 1950}},
		{"Andrew", "Elizabeth II", M, yes, {19, 2, 1960}},
		{"Edward", "Elizabeth II", M, yes, {10, 3, 1964}},
		{"David", "Margaret", M, yes, {3, 11, 1961}},
		{"Sarah", "Margaret", F, yes, {1, 5, 1964}},
		{"William", "Charles III", M, yes, {21, 6, 1982}},
		{"Henry", "Charles III", M, yes, {15, 9, 1984}},
		{"Peter", "Anne", M, yes, {15, 11, 1977}},
		{"Zara", "Anne", F, yes, {15, 5, 1981}},
		{"Beatrice", "Andrew", F, yes, {8, 8, 1988}},
		{"Eugenie", "Andrew", F, yes, {23, 3, 1990}},
		{"James", "Edward", M, yes, {17, 12, 2007}},
		{"Louise", "Edward", F, yes, {8, 11, 2003}},
		{"Charles", "David", M, yes, {1, 7, 1999}},
		{"Margarita", "David", F, yes, {14, 5, 2002}},
		{"Samuel", "Sarah", M, yes, {28, 7, 1996}},
		{"Arthur", "Sarah", M, yes, {6, 5, 2019}},
		{"George", "William", M, yes, {22, 7, 2013}},
		{"George VI", NULL, M, no, {14, 12, 1895}},
		{"Charlotte", "William", F, yes, {2, 5, 2015}},
		{"Louis", "William", M, yes, {23, 4, 2018}},
		{"Archie", "Henry", M, yes, {6, 5, 2019}},
		{"Lilibet", "Henry", F, yes, {4, 6, 2021}},
		{"Savannah", "Peter", F, yes, {29, 12, 2010}},
		{"Isla", "Peter", F, yes, {29, 3, 2012}},
		{"Mia", "Zara", F, yes, {17, 1, 2014}},
		{"Lena", "Zara", F, yes, {18, 6, 2018}},
		{"Elizabeth II", "George VI", F, no, {21, 4, 1925}},
		{"Margaret", "George VI", F, no, {21, 8, 1930}},
		{"Lucas", "Zara", M, yes, {21, 3, 2021}},
		{"Sienna", "Beatrice", F, yes, {18, 9, 2021}},
		{"August", "Eugenie", M, yes, {9, 2, 2021}}
	};

	int to_do, n, days;
	Food food_tab[FOOD_MAX];
	Date curr_date;
	char buff[ART_MAX];

	fgets(buff, ART_MAX, stdin);
	sscanf(buff, "%d", &to_do);

	switch (to_do) {
		case 1:
			fgets(buff, ART_MAX, stdin);
			sscanf(buff, "%d", &n);

			n = read_goods(food_tab, n, stdin, 1);
			scanf("%s", buff);

			print_art(food_tab, n, buff);
			break;
		case 2:
			fgets(buff, ART_MAX, stdin);
			sscanf(buff, "%d", &n);

			n = read_goods(food_tab, n, stdin, 0);

			scanf("%d %d %d %d", &curr_date.day, &curr_date.month, &curr_date.year, &days);
			printf("%.2f\n", value(food_tab, (size_t)n, curr_date, days));

			break;
		case 3:
			scanf("%d", &n);

			create_list(person_tab, sizeof(person_tab) / sizeof(Person));
			printf("%s\n", person_tab[n - 1].name);

			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
	}
	return 0;
}