#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))
#define MAX_LINE 128

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

int count[MAX_BIGRAMS] = { 0 };

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb;	return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count[vb] - count[va];
}

char isEndingChar(char x) {
	return x == '\n' || x == ' ' || x == '\t';
}

// count lines, words & chars in a given text file
void wc(int *nl, int *nw, int *nc) {
	*nl = *nw = *nc = 0;

	char currentChar, lastChar = '\n';
	while (!feof(stdin)) {
		currentChar = fgetc(stdin);
		if (currentChar == '\0') continue;

		if (currentChar == '\n') {
			(*nl)++;
		}

		if (!isEndingChar(lastChar) && isEndingChar(currentChar)) {
			(*nw)++;
		}

		(*nc)++;
		lastChar = currentChar;
	}
}

void char_count(int char_no, int *n_char, int *cnt) {
	int counters[MAX_CHARS] = { 0 };

	char currentChar;
	while (!feof(stdin)) {
		currentChar = fgetc(stdin);
		if (currentChar == '\0') continue;

		if (FIRST_CHAR <= currentChar && currentChar < LAST_CHAR) {
			counters[currentChar - FIRST_CHAR]++;
		}
	}

	int indeces[MAX_CHARS];
	for (int i = 0; i < MAX_CHARS; i++) {
		indeces[i] = i;
	}

	int temp;
	for (int i = 0; i < MAX_CHARS; i++) {
		for (int j = 1; j < MAX_CHARS - i; j++) {
			if (counters[indeces[j - 1]] < counters[indeces[j]]) {
				temp = indeces[j];
				indeces[j] = indeces[j - 1];
				indeces[j - 1] = temp;
			}
		}
	}

	*n_char = indeces[char_no - 1] + FIRST_CHAR;
	*cnt = counters[indeces[char_no - 1]];
}

void bigram_count(int bigram_no, int bigram[]) {
	char counters[MAX_BIGRAMS] = { 0 };

	char buffer[MAX_LINE], lastChar = fgetc(stdin);
	int counter, j, index;
	while (fgets(buffer, MAX_LINE, stdin) != NULL) {
		for (int i = 0; i < MAX_LINE; i++) {
			if (buffer[i] == '\0') break;

			if (buffer[i] < FIRST_CHAR) {
				lastChar = '\0';
				continue;
			}

			if (lastChar != '\0') {
				index = (lastChar - FIRST_CHAR) * MAX_CHARS + (buffer[i] - FIRST_CHAR);

				if (lastChar == buffer[i]) {
					counter = 1; j = i + 1;
					while (buffer[j - 1] == buffer[j++]) {
						counter++;
					}

					counters[index] += (counter + 1) / 2;
					i = j - 2;
				} else {
					counters[index]++;
				}
			}

			lastChar = buffer[i];
		}
	}

	int indeces[MAX_BIGRAMS];
	for (int i = 0; i < MAX_BIGRAMS; i++) {
		indeces[i] = i;
	}

	int temp;
	for (int i = 0; i < MAX_BIGRAMS; i++) {
		for (int j = 1; j < MAX_BIGRAMS - i; j++) {
			if (counters[indeces[j - 1]] < counters[indeces[j]]) {
				temp = indeces[j];
				indeces[j] = indeces[j - 1];
				indeces[j - 1] = temp;
			}
		}
	}

	bigram[0] = (indeces[bigram_no - 1] / MAX_CHARS) + FIRST_CHAR;
	bigram[1] = (indeces[bigram_no - 1] % MAX_CHARS) + FIRST_CHAR;
	bigram[2] = counters[indeces[bigram_no - 1]];
}

void find_comments(int *line_comment_counter, int *block_comment_counter) {
	*line_comment_counter = *block_comment_counter = 0;

	char buffer[MAX_LINE], lastChar = '\0';
	int lineComment = 0, blockComment = 0;
	while (fgets(buffer, MAX_LINE, stdin) != NULL) {
		for (int i = 0; i < MAX_LINE; i++) {
			if (buffer[i] == '\0') break;

			if (lastChar == '/' && buffer[i] == '*' && blockComment == 0 && lineComment == 0) {
				blockComment++;
				(*block_comment_counter)++;
			} else if (lastChar == '*' && buffer[i] == '/' && blockComment > 0) {
				blockComment--;
			} else if (lastChar == '/' && buffer[i] == '/' && blockComment == 0 && lineComment == 0) {
				lineComment++;
				(*line_comment_counter)++;
			} else if (buffer[i] == '\n' && blockComment == 0 && lineComment > 0) {
				lineComment--;
			} else {
				i--;
			}

			lastChar = buffer[++i];
		}
	}
}

int read_int() {
	char line[MAX_LINE];
	fgets(line, MAX_LINE, stdin); // to get the whole line
	return (int)strtol(line, NULL, 10);
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int bigram[3];

	to_do = read_int();
	switch (to_do) {
		case 1: // wc()
			wc (&nl, &nw, &nc);
			printf("%d %d %d\n", nl, nw, nc);
			break;
		case 2: // char_count()
			char_no = read_int();
			char_count(char_no, &n_char, &cnt);
			printf("%c %d\n", n_char, cnt);
			break;
		case 3: // bigram_count()
			char_no = read_int();
			bigram_count(char_no, bigram);
			printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
			break;
		case 4:
			find_comments(&line_comment_counter, &block_comment_counter);
			printf("%d %d\n", block_comment_counter, line_comment_counter);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}
