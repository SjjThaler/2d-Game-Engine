#include <stdio.h>
#include <math.h>

typedef struct {
	float x;
	float y;
} Point;

Point add(Point one, Point two) {
	Point addition;
	addition.x = one.x + two.x;
	addition.y = one.y + two.y;
	return addition;
}

float distance(Point one, Point two) {
	float dx = two.x-one.x;
	float dy = two.y-one.y;
	float dist = sqrtf(dx*dx + dy*dy);	
	return dist;
}

void move(Point *p, float dx, float dy) {
	p->x = p->x + dx;
	p->y = p->y + dy;
}

void print_point(const Point *p) {
	printf("x: %.2f, y:%.2f\n", p->x, p->y);
}

int main(void) {
	Point one = {1.0f, 1.0f};
	Point two = {5.0f, 5.0f};
	
	Point result = add(one,two);
	print_point(&result);

	float dist_test = distance(one,two);
	printf("The distance is: %.2f\n", dist_test);

	move(&one, 10.0f, 10.0f);
	print_point(&one);
	return 0;
}
