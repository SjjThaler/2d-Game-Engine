#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define HEIGHT 20
#define WIDTH 40
#define OUTPUTS 5
#define INPUTS 4
#define POP_SIZE 500
#define SURVIVAL_LIMIT 100
#define GENERATIONS 100

struct dot {
	int x;
	int y;

};

struct brain {
	float weights[OUTPUTS][INPUTS];
	float biases[OUTPUTS];
};


struct brain active;		

enum action {
	UP = 1,
	DOWN = 2,
	RIGHT = 3,
	LEFT = 4,
	STAY = 0
};





enum action forward_pass(int inputs[INPUTS]) {
	float score[5];
	for(int j = 0; j < OUTPUTS; j++) {
		score[j] = active.biases[j];
		for(int i = 0; i < INPUTS; i++) {
			score[j] += inputs[i] * active.weights[j][i];	
		}
	}
	int best = 0;
	for (int k = 0; k < OUTPUTS; k++) {
		if (score[k] > score[best]) {
			best = k;
		}

	}
	enum action nn_move = best;
	return nn_move;
}

enum action player_controller (struct dot player, struct dot opponent) {
	int distance_x = opponent.x - player.x;
	int  distance_y = opponent.y - player.y;
	if (abs(distance_x) >= abs(distance_y)) {
		if (distance_x > 0) {
			return RIGHT;
		}
		else if (distance_x < 0) {
			return LEFT;	
		}	
		else {
			return STAY;
		}
	}
	else {
		if (distance_y > 0) {
			return DOWN;
		}
		else if (distance_y < 0) {
			return UP;
		}
		else {
			return STAY;
		}
	}
}
enum action opponent_controller (struct dot player, struct dot opponent) {
	int inputs[INPUTS] = {opponent.x-player.x, opponent.y-player.y, opponent.x, opponent.y}; 
	enum action nn_move = forward_pass(inputs);	
	return nn_move;	
}

void apply_action(struct dot *dot, enum action action ) {
	switch (action) {
			case UP: dot->y -= 1; break;
			case DOWN: dot->y += 1; break;
			case RIGHT: dot->x += 1; break;
			case LEFT: dot->x -= 1; break;
			case STAY: break;
	}
	if (dot->x > WIDTH-1) {
		dot->x = WIDTH-1;
	}
	if (dot->x < 0) {
		dot->x = 0;
	}
	if (dot->y > HEIGHT-1) {
		dot->y = HEIGHT-1;
	}
	if (dot->y < 0) {
		dot->y = 0;
	}	
}

void fill_weights_biases(struct brain *b) {
	for (int i = 0; i < OUTPUTS; i++) {
		for (int j = 0; j < INPUTS; j++) {
			
			b->weights[i][j] = (float)rand() / RAND_MAX * 2 - 1;
		}
	}
	for (int k = 0; k < OUTPUTS; k++) {
		b->biases[k] = (float)rand() / RAND_MAX * 2 - 1;
	}
}

void mutate(struct brain *b) {
	for (int i = 0; i < OUTPUTS; i++) {
		for (int j = 0; j < INPUTS; j++) {
			
			b->weights[i][j] += ((float)rand() / RAND_MAX * 2 - 1) * 0.1;
		}
	}
	for (int k = 0; k < OUTPUTS; k++) {
		b->biases[k] += ((float)rand() / RAND_MAX * 2 - 1) * 0.1;
	}	
}

int tournament(int fitness[]) {
	int random_brains[3];
	for (int brain = 0; brain < 3; brain ++) {
		random_brains[brain] = rand() % POP_SIZE;
	}
	int winner = 0;
	for (int brain = 0; brain < 3; brain ++) {
		if (fitness[random_brains[brain]] > fitness[random_brains[winner]]) {
			winner = brain;
		}
	}
	return random_brains[winner];
}
int run_match(int render) {
	struct dot player = {
		0,0,	
	};
	struct dot opponent = {
		10,10,
	};
	int survival_count = 0;
	while(survival_count < SURVIVAL_LIMIT) {
		// Render block if render bit is flipped
		if (render) {
			usleep(200000);
			printf("\033[2J");
			printf("\033[H");
			
			for (int row = 0; row < HEIGHT; row ++) {
				for (int column = 0; column < WIDTH; column ++) {
					if (row == 0 || row == HEIGHT -1) {
						printf("#");
					}
					else if (column == 0 || column == WIDTH -1) {
						printf("#");
					}
					else if (player.y == row && player.x == column) {
						printf("x");
					}
					else if (opponent.y == row && opponent.x == column) {
						printf("0");
					}
					else {
						printf(" ");
					}
				}
			printf("\n");
			}
		}

		// Escapee gets one move
		enum action opponent_choice = opponent_controller(player, opponent);
		apply_action(&opponent, opponent_choice);		
		// Pursuer gets 1.5 moves
		enum action player_choice = player_controller(player, opponent);
		apply_action(&player, player_choice);
		if (player.x == opponent.x && player.y == opponent.y) {
			return survival_count;
		}
		if (survival_count % 2 != 0) {
			player_choice = player_controller(player, opponent);
			apply_action(&player, player_choice);
			if (player.x == opponent.x && player.y == opponent.y) {
				return survival_count;
			}
		}
		survival_count += 1;
	}

	return survival_count;
}

int main (void) {
		srand(time(NULL));
		struct brain population[POP_SIZE];
		int fitness[POP_SIZE];

		for (int i = 0; i < POP_SIZE; i++) {
			fill_weights_biases(&population[i]);
			active = population[i];
			fitness[i] = run_match(0);
		}

		for(int g = 0; g < GENERATIONS; g++) {
			for (int p = 0; p < POP_SIZE; p++) {
				active = population[p];
				fitness[p] = run_match(0);
			}
			int best = 0;
			int average = 0;
			for (int i = 0; i < POP_SIZE; i++) {
				if (fitness[i] > fitness[best]) {
					best = i;
				}
				average += fitness[i];	
			}
			average = average / POP_SIZE;
			

			struct brain champion = population[best];
			struct brain next_gen[POP_SIZE];
			next_gen[0] = champion;
				
			for (int i = 1; i < POP_SIZE; i++) {
				//struct brain mutant = champion;
				int winner = tournament(fitness);
				struct brain mutant = population[winner];
				mutate(&mutant);
				next_gen[i] = mutant;
			}
			for (int i = 0; i < POP_SIZE; i++) {
				population[i] = next_gen[i];
			}
			printf("Average of generation %d: %d\n", g, average);
		}
		active = population[0];
		int final = run_match(1);
		return 0;
}
