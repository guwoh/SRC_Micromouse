#include <stdio.h>
#include "solver.h"
#include "API.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function
#include <time.h> // For random number generation

int MAZE_SIZE = 16; // Example size, adjust as needed

int main(int argc, char* argv[]) {
    debug_log("Running...");
    while (1) {
        Action nextMove = solver();
        switch(nextMove){
            case FORWARD: {
                API_moveForward();
                switch (robot.dir) {
                    case NORTH: robot.y++; break;
                    case EAST:  robot.x++; break;
                    case SOUTH: robot.y--; break;
                    case WEST:  robot.x--; break;
                }
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "Moving to (%d, %d, %c)", robot.x, robot.y, robot.dir);
                debug_log(buffer);
                break;
            }
            case LEFT: {
                API_turnLeft();
                robot.dir = (robot.dir + 3) % 4;  // -1 + 4 mod 4
                switch (robot.dir) {
                    case 'N': robot.dir = 'E'; break;
                    case 'E': robot.dir = 'S'; break;
                    case 'S': robot.dir = 'W'; break;
                    case 'W': robot.dir = 'N'; break;
                }
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "Moving to (%d, %d, %d)", robot.x, robot.y, robot.dir);
                debug_log(buffer);
                break;
            } 
            case RIGHT: {
                API_turnRight();
                robot.dir = (robot.dir + 1) % 4;
                switch (robot.dir) {
                    case 'N': robot.dir = 'W'; break;
                    case 'W': robot.dir = 'S'; break;
                    case 'S': robot.dir = 'E'; break;
                    case 'E': robot.dir = 'N'; break;
                }
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "Moving to (%d, %d, %c)", robot.x, robot.y, robot.dir);
                debug_log(buffer);
                break;
            }
            case IDLE:
                break;
        }
    }
}
