#pragma once
// API.h
// This header file defines the API functions for interacting with a maze simulation environment.
// The functions allow the user to query the maze dimensions, check for walls, move within the maze, and manipulate colors and text on the maze grid.
// The API is designed to be used in a maze-solving context, where the user can navigate through the maze and perform various operations.
// The API functions are implemented in API.c, and this header file provides the function prototypes for those implementations.
int API_mazeWidth();
int API_mazeHeight();

int API_wallFront();
int API_wallRight();
int API_wallLeft();

int API_moveForward();  // Returns 0 if crash, else returns 1
void API_turnRight();
void API_turnLeft();

void API_setWall(int x, int y, char direction);
void API_clearWall(int x, int y, char direction);

void API_setColor(int x, int y, char color);
void API_clearColor(int x, int y);
void API_clearAllColor();

void API_setText(int x, int y, char* str);
void API_clearText(int x, int y);
void API_clearAllText();

int API_wasReset();
void API_ackReset();

void debug_log(char* text);