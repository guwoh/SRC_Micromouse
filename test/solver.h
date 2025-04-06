#ifndef SOLVER_H
#define SOLVER_H

typedef enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3} Direction;
typedef enum Color {WHITE, BLACK, RED, GREEN, BLUE, YELLOW} Color;
typedef enum Wall {WALL_FRONT, WALL_RIGHT, WALL_LEFT} Wall;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

Action solver();
Action rightWallFollower();
Action floodFill();
Action leftWallFollower();


// Directions are represented as 'N', 'E', 'S', 'W'
typedef struct {
    int x;
    int y;
    Direction dir; // 'N', 'E', 'S', 'W'
} Point;

extern Point robot; //= {0, 0, 'N'}; // Initialize the robot's position and direction
#endif