#include <stdio.h>
#include <stdlib.h>

struct dot {
	int x;
	int y;

};
enum action {
	UP = 1,
	DOWN = 2,
	RIGHT = 3,
	LEFT = 4,
	STAY = 0
};

enum action opponent_controller (struct dot player, struct dot opponent) {
	
	return STAY;	
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

void apply_action(struct dot *dot, enum action action ) {
	switch (action) {
			case UP: dot->y -= 1; break;
			case DOWN: dot->y += 1; break;
			case RIGHT: dot->x += 1; break;
			case LEFT: dot->x -= 1; break;
			case STAY: break;
	}

}

int main (void) {
	struct dot player = {
		0,0,	
	};
	struct dot opponent = {
		10,10,
	};
	int survival_count = 0;
	while(survival_count < 10) {

		enum action opponent_choice = opponent_controller(player, opponent);
		apply_action(&opponent, opponent_choice);		
		enum action player_choice = player_controller(player, opponent);
		apply_action(&player, player_choice);
			
		if (player.x == opponent.x && player.y == opponent.y) {
			printf("Win!\nOpponent survived for %d rounds!", survival_count);
			return 0;
		}
		survival_count += 1;
		printf("player: %d,%d\nopponent: %d,%d\n", player.x,player.y,opponent.x,opponent.y);
	}
	printf("Game over!\n");
	return 0;
}
