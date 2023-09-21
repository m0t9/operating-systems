#include <math.h>
#include <stdio.h>

#define sqr(x) ((x) * (x))

struct Point {
    float x;
    float y;
};

typedef struct Point Point;

float distance(Point a, Point b) {
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

float area(Point a, Point b, Point c) {
    return 0.5f * fabs(a.x * b.y - b.x * a.y + 
            b.x * c.y - c.x * b.y + c.x * a.y - a.x * c.y);
}

int main() {
   Point A = {.x = 2.5f, .y = 6.0f},
         B = {.x = 1.0f, .y = 2.2f},
         C = {.x = 10.0f, .y = 6.0f};
   printf("The distance between A and B is %f\n", distance(A, B));
   printf("The area of ABC triangle is %f\n", area(A, B, C));
   return 0;
}
