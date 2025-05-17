#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define SCREEN_WIDTH 151
#define SCREEN_HEIGHT 40
#define WALL_BLOCK_DISTANCE 0.25
#define FOV 0.25
#define VIEW_DISTANCE 20
#define GRID_X 30
#define GRID_Y 30
#define WIDTH_ESCAPE 22
#define BUFFER_SIZE (SCREEN_HEIGHT * (SCREEN_WIDTH * (WIDTH_ESCAPE + 1)) + 1)
#define BUFFER_ROW_WIDTH  ((SCREEN_WIDTH * (WIDTH_ESCAPE + 1)) + 1)

#define WALL_CHAR_1 '@'
#define WALL_CHAR_2 '#'
#define WALL_CHAR_3 '&'
#define WALL_CHAR_4 '$'
#define WALL_CHAR_5 '%'
#define WALL_CHAR_6 'x'
#define WALL_CHAR_7 '!'
#define WALL_CHAR_8 ':'
#define WALL_CHAR_9 '.'
#define WALL_CHAR_10 ' '
#define WALL_CHAR_BLANK '?'

struct screen {
    char * buffer[2];
    int current_buffer;    
};

struct camera {
    float pos_x;
    float pos_y;
    float walk_speed;
    double angle;
    float turn_speed;
    int turning;
    int walking;
};

void switch_buffer(struct screen * scr);
void clear_buffer(struct screen * scr);
void get_ray_dir(struct camera * cam, float * ray_dir_x, float *ray_dir_y, int screen_offset);
float cast_ray(float pos_x, float pos_y, float ray_dir_x, float ray_dir_y, 
    int grid[], int * grid_index);
void render_column(char buffer[], int column, float distance);
void render_screen(struct camera * cam, struct screen * scr, int grid[]);
int grid_to_index(int x, int y, int newlines);
int pixel_to_index(int x, int y);
void get_grid(int grid[], int selection);
void int_to_char_code(int index, char *hundred, char *ten, char *one);

#endif