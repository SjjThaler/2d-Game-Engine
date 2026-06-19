#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define HEIGHT 40
#define WIDTH 84

typedef struct {
	int x;
	int y;	
} Cell;

struct termios original;

void enable_raw_mode(void) {
	tcgetattr(STDIN_FILENO, &original);
	struct termios raw = original;
	raw.c_lflag &= ~(ICANON | ECHO);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &original);
	printf("\033[?25h");
}

void render (const Cell *head) {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			if (row == 0 || row == HEIGHT - 1) {
				printf("#");
			}
			else if (col == 0 || col == WIDTH - 1) {
				printf("#");
			}
			else if (col == head->x && row == head->y){
				printf("0");
			}
			else{
				printf(" ");
			}
		}	
		printf("\n");
	}
}
int main(void) {
	// initialize velocity
	int dx = 1;
	int dy = 0;
	// initialize snake head
	Cell head;
	head.x = 5;
	head.y = 5;
	// setup terminal
	printf("\033[?25l");
	enable_raw_mode();
	// start game loop
	int running = 1;
	while (running) {
		char c;
		ssize_t n = read(STDIN_FILENO, &c, 1);
		if (n == 1) {
			switch (c) {
				case 'a':
					dx = -1;
					dy = 0;
					break;
				case 'd':
					dx = 1;
					dy = 0;
					break;
				case 'w':
					dx = 0;
					dy = -1;
					break;
				case 's':
					dx = 0;
					dy = 1;
					break;	
				case 'q':
					running = 0;
					break;
			}
		}
		usleep(200000);
		printf("\033[2J");
		printf("\033[H");
		render(&head);
		head.x += dx;
		head.y += dy;
	}	
	disable_raw_mode();	
	return 0;
}
