#include "solver.h"
#include "API.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define MAZE_SIZE 16
#define QUEUE_SIZE 256
#define UNVISITED 255 

// --- QUY ƯỚC ---
// Tọa độ (x, y): x là cột, y là hàng. (0,0) ở góc Tây-Nam.
// Hướng (Heading): NORTH=0, EAST=1, SOUTH=2, WEST=3.
// Tường (Walls):
// - h_walls[y][x]: Tường ngang PHÍA TRÊN ô (x,y).
// - v_walls[x][y]: Tường dọc BÊN PHẢI ô (x,y).

// --- BIẾN TOÀN CỤC ---
int distances[MAZE_SIZE][MAZE_SIZE];
int h_walls[MAZE_SIZE][MAZE_SIZE];
int v_walls[MAZE_SIZE][MAZE_SIZE];

// --- HÀNG ĐỢI (QUEUE) CHO FLOODFILL ---
typedef struct {
    Coord queue[QUEUE_SIZE];
    int front, rear, count;
} Queue;

Queue q;

void init_queue() { q.front = 0; q.rear = -1; q.count = 0; }
bool is_empty() { return q.count == 0; }
void enqueue(Coord c) {
    q.rear = (q.rear + 1) % QUEUE_SIZE;
    q.queue[q.rear] = c;
    q.count++;
}
Coord dequeue() {
    Coord c = q.queue[q.front];
    q.front = (q.front + 1) % QUEUE_SIZE;
    q.count--;
    return c;
}

// --- CÁC HÀM TIỆN ÍCH ---
void init_maze_data() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            distances[i][j] = UNVISITED;
            h_walls[i][j] = 0;
            v_walls[i][j] = 0;
        }
    }
}

void update_walls(Coord pos, Heading heading) {
    int x = pos.x;
    int y = pos.y;
    if (API_wallFront()) {
        switch (heading) {
            case NORTH: if (y < MAZE_SIZE - 1) h_walls[y][x] = 1; break;
            case EAST:  if (x < MAZE_SIZE - 1) v_walls[x][y] = 1; break;
            case SOUTH: if (y > 0) h_walls[y - 1][x] = 1; break;
            case WEST:  if (x > 0) v_walls[x - 1][y] = 1; break;
        }
    }
    if (API_wallLeft()) {
        switch (heading) {
            case NORTH: if (x > 0) v_walls[x - 1][y] = 1; break;
            case EAST:  if (y < MAZE_SIZE - 1) h_walls[y][x] = 1; break;
            case SOUTH: if (x < MAZE_SIZE - 1) v_walls[x][y] = 1; break;
            case WEST:  if (y > 0) h_walls[y - 1][x] = 1; break;
        }
    }
    if (API_wallRight()) {
        switch (heading) {
            case NORTH: if (x < MAZE_SIZE - 1) v_walls[x][y] = 1; break;
            case EAST:  if (y > 0) h_walls[y - 1][x] = 1; break;
            case SOUTH: if (x > 0) v_walls[x - 1][y] = 1; break;
            case WEST:  if (y < MAZE_SIZE - 1) h_walls[y][x] = 1; break;
        }
    }
}

// --- THUẬT TOÁN FLOOD FILL ---
void floodfill() {
    API_clearAllText(); // Xóa các số cũ trước khi tính toán lại
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            distances[i][j] = UNVISITED;
        }
    }
    init_queue();

    Coord goals[] = { {7,7}, {8,7}, {7,8}, {8,8} };
    for (int i = 0; i < 4; i++) {
        distances[goals[i].x][goals[i].y] = 0;
        enqueue(goals[i]);
        API_setText(goals[i].x, goals[i].y, "0"); // Hiển thị số 0 cho ô đích
    }

    int dx[] = {0, 1, 0, -1}; // N, E, S, W
    int dy[] = {1, 0, -1, 0};
    while (!is_empty()) {
        Coord current = dequeue();
        int x = current.x, y = current.y;
        int dist = distances[x][y];

        for (int i = 0; i < 4; i++) {
            int new_x = x + dx[i], new_y = y + dy[i];
            if (new_x < 0 || new_x >= MAZE_SIZE || new_y < 0 || new_y >= MAZE_SIZE) continue;

            bool wall_exists = false;
            switch (i) {
                case NORTH: if (y < MAZE_SIZE - 1 && h_walls[y][x]) wall_exists = true; break;
                case EAST:  if (x < MAZE_SIZE - 1 && v_walls[x][y]) wall_exists = true; break;
                case SOUTH: if (y > 0 && h_walls[y - 1][x]) wall_exists = true; break;
                case WEST:  if (x > 0 && v_walls[x - 1][y]) wall_exists = true; break;
            }
            
            if (!wall_exists && distances[new_x][new_y] == UNVISITED) {
                distances[new_x][new_y] = dist + 1;
                enqueue((Coord){new_x, new_y});

                // HIỂN THỊ SỐ LÊN Ô MÊ CUNG
                char buffer[5];
                sprintf(buffer, "%d", dist + 1);
                API_setText(new_x, new_y, buffer);
            }
        }
    }
}

// --- HÀM SOLVER CHÍNH ---
Action solver() {
    static Coord pos = {0, 0};
    static Heading heading = NORTH;
    
    if (API_wasReset()) {
        API_ackReset();
        pos.x = 0; pos.y = 0;
        heading = NORTH;
        init_maze_data();
        API_clearAllColor(); // Xóa màu khi reset
    }
    
    update_walls(pos, heading);
    floodfill();

    if (distances[pos.x][pos.y] == 0) {
        return IDLE;
    }

    int current_dist = distances[pos.x][pos.y];
    int min_neighbor_dist = current_dist;
    int best_direction = -1;

    if (pos.y < MAZE_SIZE - 1 && !h_walls[pos.y][pos.x] && distances[pos.x][pos.y + 1] < min_neighbor_dist) {
        min_neighbor_dist = distances[pos.x][pos.y + 1];
        best_direction = NORTH;
    }
    if (pos.x < MAZE_SIZE - 1 && !v_walls[pos.x][pos.y] && distances[pos.x + 1][pos.y] < min_neighbor_dist) {
        min_neighbor_dist = distances[pos.x + 1][pos.y];
        best_direction = EAST;
    }
    if (pos.y > 0 && !h_walls[pos.y - 1][pos.x] && distances[pos.x][pos.y - 1] < min_neighbor_dist) {
        min_neighbor_dist = distances[pos.x][pos.y - 1];
        best_direction = SOUTH;
    }
    if (pos.x > 0 && !v_walls[pos.x - 1][pos.y] && distances[pos.x - 1][pos.y] < min_neighbor_dist) {
        min_neighbor_dist = distances[pos.x - 1][pos.y];
        best_direction = WEST;
    }

    Action move;
    if (best_direction != -1) {
        if (best_direction == heading) {
            move = FORWARD;
            if (heading == NORTH) pos.y++;
            else if (heading == EAST) pos.x++;
            else if (heading == SOUTH) pos.y--;
            else if (heading == WEST) pos.x--;
        } else if (best_direction == (heading + 1) % 4) {
            move = RIGHT;
            heading = (Heading)((heading + 1) % 4);
        } else {
            move = LEFT;
            heading = (Heading)((heading + 3) % 4);
        }
    } else {
        move = LEFT;
        heading = (Heading)((heading + 3) % 4);
    }
    
    // TÔ MÀU CHO Ô HIỆN TẠI
    API_setColor(pos.x, pos.y, 'B'); // 'B' for Blue
    
    return move;
}