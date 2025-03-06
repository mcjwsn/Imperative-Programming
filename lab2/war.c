#include <stdio.h>
#include <stdlib.h>

#define DEQUE_SIZE 52
#define PLAYER_COUNT 2

typedef struct {
	int content[DEQUE_SIZE],
		start, size;
} Deque;

typedef struct {
	int content[DEQUE_SIZE],
		size;
} Stack;

void stackAppend(Stack *stack, int card) {
	if ((*stack).size == DEQUE_SIZE) {
		return;
	}

	(*stack).content[(*stack).size++] = card;
}

int stackPop(Stack *stack) {
	if ((*stack).size == 0) {
		return -1;
	}

	return (*stack).content[--(*stack).size];
}

void dequeAppend(Deque *deque, int card) {
	if ((*deque).size == DEQUE_SIZE) {
		return;
	}

	int index = ((*deque).start + (*deque).size++) % DEQUE_SIZE;
	(*deque).content[index] = card;
}

int dequePop(Deque *deque) {
	if ((*deque).size == 0) {
		return -1;
	}

	int result = (*deque).content[(*deque).start++];
	(*deque).start %= DEQUE_SIZE;
	(*deque).size--;

	return result;
}

void dequePrint(Deque *deque) {
	for (int i = 0; i < (*deque).size; i++) {
		printf("%d ", (*deque).content[((*deque).start + i) % DEQUE_SIZE]);
	}

	printf("\n");
}

void generateCards(Deque *player1, Deque *player2) {
	int deque[DEQUE_SIZE];
	for (int i = 0; i < DEQUE_SIZE; i++) {
		deque[i] = i;
	}

	int k, temp;
	for (int i = 0; i < DEQUE_SIZE - 1; i++) {
		k = (rand() % (DEQUE_SIZE - i)) + i;

		temp = deque[k];
		deque[k] = deque[i];
		deque[i] = temp;
	}

	for (int i = 0; i < DEQUE_SIZE / 2; i++) {
		dequeAppend(player1, deque[i]);
		dequeAppend(player2, deque[i + (DEQUE_SIZE / 2)]);
	}
}

int winningPlayer(Stack *player1Stack, Stack *player2Stack, int *conflictCounter) {
	int card1 = stackPop(player1Stack);
	int card2 = stackPop(player2Stack);
	stackAppend(player1Stack, card1);
	stackAppend(player2Stack, card2);

	(*conflictCounter)++;
	card1 >>= 2;
	card2 >>= 2;

	if (card1 > card2) {
		return 1;
	} else if (card1 < card2) {
		return 2;
	}

	return 0;
}

void awardCardsToWinner(Stack *winningPlayer, Stack *loosingPlayer, Deque *winner) {
	Stack cardStack;
	cardStack.size = 0;

	while ((*winningPlayer).size > 0) {
		stackAppend(&cardStack, stackPop(winningPlayer));
	}

	while (cardStack.size > 0) {
		dequeAppend(winner, stackPop(&cardStack));
	}

	while ((*loosingPlayer).size > 0) {
		stackAppend(&cardStack, stackPop(loosingPlayer));
	}

	while (cardStack.size > 0) {
		dequeAppend(winner, stackPop(&cardStack));
	}
}

void returnCardsToPlayers(Stack *player1Stack, Deque *player1, Stack *player2Stack, Deque *player2) {
	while ((*player1Stack).size > 0) {
		dequeAppend(player1, stackPop(player1Stack));
	}

	while ((*player2Stack).size > 0) {
		dequeAppend(player2, stackPop(player2Stack));
	}
}

int playGame(Deque *player1, Deque *player2, int *gameType, int conflictLimit, int *conflictCount) {
	Stack player1Stack, player2Stack;
	player1Stack.size = 0; player2Stack.size = 0;

	int winner;
	while (*conflictCount <= conflictLimit && (*player1).size > 0 && (*player2).size > 0) {
		stackAppend(&player1Stack, dequePop(player1));
		stackAppend(&player2Stack, dequePop(player2));

		winner = winningPlayer(&player1Stack, &player2Stack, conflictCount);
		if (winner == 0 && *gameType == 1) {
			returnCardsToPlayers(&player1Stack, player1, &player2Stack, player2);
			continue;
		}

		while (winner == 0 && *conflictCount <= conflictLimit && (*player1).size > 0 && (*player2).size > 0) {
			stackAppend(&player1Stack, dequePop(player1));
			stackAppend(&player2Stack, dequePop(player2));

			if ((*player1).size > 0 && (*player2).size > 0) {
				stackAppend(&player1Stack, dequePop(player1));
				stackAppend(&player2Stack, dequePop(player2));

				winner = winningPlayer(&player1Stack, &player2Stack, conflictCount);
			}
		}

		switch (winner) {
			case 0:
				returnCardsToPlayers(&player1Stack, player1, &player2Stack, player2);
				return *conflictCount <= conflictLimit;
			case 1:
				awardCardsToWinner(&player1Stack, &player2Stack, player1);
				break;
			case 2:
				awardCardsToWinner(&player2Stack, &player1Stack, player2);
				break;
			default: return -1;
		}
	}

	return winner + 1;
}

int main() {
	int seed, gameType, conflictLimit;
	scanf("%d %d %d", &seed, &gameType, &conflictLimit);
	srand((unsigned) seed);

	Deque player1, player2;
	player1.start = 0; player2.start = 0;
	player1.size = 0; player2.size = 0;

	generateCards(&player1, &player2);

	int conflicts = 0;
	switch (playGame(&player1, &player2, &gameType, conflictLimit, &conflicts)) {
		case 0:
			printf("0 %d %d\n", player1.size, player2.size);
			break;
		case 1:
			printf("1 %d %d\n", player1.size, player2.size);
			break;
		case 2:
			printf("2 %d\n", conflicts);
			break;
		case 3:
			printf("3\n");
			dequePrint(&player2);
	}

	return 0;
}