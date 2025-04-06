#include "solver.h"
#include "API.h"

Action solver() {
    return leftWallFollower();
}

// This is an example of a simple left wall following algorithm.
Action leftWallFollower() {
    if(API_wallFront()) {
        if(API_wallLeft()){
            return RIGHT;
        }
        return LEFT;
    }
    return FORWARD;
}


// Put your implementation of floodfill here!
Action floodFill() {
    return IDLE;
}


Point robot = {0, 0, 'N'}; // Initialize the robot's position and direction

//      N 0 
//      |
//  W 3 --+-- E 1
//      |
//      S 2
// Directions are represented as 'N', 'E', 'S', 'W'
// The robot starts facing North (upwards in the maze)
// The robot's position is represented by (x, y) coordinates
// The robot's direction is represented by a character ('N', 'E', 'S', 'W')
// The robot can turn left or right, and move forward in the direction it is facing

void turnLeft() {
    API_turnLeft();
    switch (robot.dir) {
        case 0 : robot.dir = 3; break;
        case 1: robot.dir =  0; break;
        case 2: robot.dir =  1; break;
        case 3: robot.dir =  2; break;
    }
}


void turnRight() {
    API_turnRight();
    switch (robot.dir) {
        case 'N': robot.dir = 'E'; break;
        case 'E': robot.dir = 'S'; break;
        case 'S': robot.dir = 'W'; break;
        case 'W': robot.dir = 'N'; break;
    }
}

void moveForward() {
    if (API_moveForward()) {
        switch (robot.dir) {
            case 'N': robot.y++; break; // Move up
            case 'E': robot.x++; break; // Move right
            case 'S': robot.y--; break; // Move down
            case 'W': robot.x--; break; // Move left
        }
    } else {
        // Handle collision (e.g., stop or turn)
    }
    switch (robot.dir) {
        case 'N': robot.dir = 'E'; break;
        case 'E': robot.dir = 'S'; break;
        case 'S': robot.dir = 'W'; break;
        case 'W': robot.dir = 'N'; break;
    }
}