#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

typedef struct {
    int x, y;
} Coord;

void init_maze();
void floodfill();
Action get_next_move(Coord current, Heading heading);
Action solver();
Action leftWallFollower();
Action floodFill();

extern int distances[16][16];
extern int h_walls[16][16];
extern int v_walls[16][16];

#endif