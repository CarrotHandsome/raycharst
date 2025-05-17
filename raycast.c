#include "raycast.h"
#include <math.h>

void clear_buffer(struct screen * scr) {
    int i = 0;
    memset(scr->buffer[scr->current_buffer], WALL_CHAR_10, BUFFER_SIZE);
    // for (int y = 0; y < SCREEN_HEIGHT; y += 1) {
    //     for (int x = 0; x < SCREEN_WIDTH; x += 1) {
    //         i = pixel_to_index(x, y);
    //         sprintf(&(scr->buffer[scr->current_buffer][i]), "\033[38;5;255m ");            
    //     }
    //     scr->buffer[scr->current_buffer][pixel_to_index(SCREEN_WIDTH, y)] = '\n';

    // }
    
    scr->buffer[scr->current_buffer][BUFFER_SIZE] = '\0';
} 
void switch_buffer(struct screen * scr) {
    if (scr->current_buffer == 0) scr->current_buffer = 1;
    else scr->current_buffer = 0;
}

float cast_ray(float pos_x, float pos_y, float ray_dir_x, float ray_dir_y, 
        int grid[], int * grid_index) {
    float delta_dist_x = abs(1 / ray_dir_x);
    float delta_dist_y = abs(1 / ray_dir_y);

    float side_dist_x;
    float side_dist_y;
    int grid_x = floor(pos_x);
    int grid_y = floor(pos_y);
    int step_x;
    int step_y;
    int side = 0;

    int hit = 0;

    
    if (ray_dir_x >= 0) {
        side_dist_x = (grid_x + 1 - pos_x) * delta_dist_x;
        step_x = 1;
    } else {
        side_dist_x = (pos_x - grid_x) * delta_dist_x;
        step_x = -1;
    }

    if (ray_dir_y >= 0) {
        side_dist_y = (grid_y + 1 - pos_y) * delta_dist_y;
        step_y = 1;
    } else {
        side_dist_y = (pos_y - grid_y) * delta_dist_y;
        step_y = -1;
    }

    while (!hit) {
        if (side_dist_x < side_dist_y) {
            side_dist_x += delta_dist_x;
            grid_x += step_x;
            side = 0;
        } else {
            side_dist_y += delta_dist_y;
            grid_y += step_y;
            side = 1;
        }
        *grid_index = grid_to_index(grid_x, grid_y, 0);
        if (grid[*grid_index]) {
            hit = 1;
        }
    }

    float distance;
    if (side == 0) distance = (grid_x - pos_x + (1 - step_x) / 2) / ray_dir_x; 
    else distance = (grid_y - pos_y + (1 - step_y) / 2) / ray_dir_y;

    return distance;    
}

void render_column(char buffer[], int column, float distance) {
    int line_height = SCREEN_HEIGHT / distance;
    int half_height = line_height / 2;
    int draw_start = SCREEN_HEIGHT / 2 - half_height;
    int draw_end = SCREEN_HEIGHT / 2 + half_height;//(line_height - 1) / 2;
    if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;
    if (draw_start < 0) draw_start = 0;
    char render_char1 = ' ';
    char render_char2 = ' ';
    int light = 0;
    if (distance >= VIEW_DISTANCE) { render_char1 = WALL_CHAR_10; render_char2 = WALL_CHAR_10; light = 237; }
    else if (distance >= 16) { render_char1 = WALL_CHAR_9; render_char2 = WALL_CHAR_9; light = 237; }
    else if (distance >= 14) { render_char1 = WALL_CHAR_9; render_char2 = WALL_CHAR_8; light = 239;}
    else if (distance >= 12.75) { render_char1 = WALL_CHAR_8; render_char2 = WALL_CHAR_8; light = 239;}
    else if (distance >= 11.5) { render_char1 = WALL_CHAR_8; render_char2 = WALL_CHAR_7; light = 241;}
    else if (distance >= 10.25) { render_char1 = WALL_CHAR_7; render_char2 = WALL_CHAR_7; light = 242;}
    else if (distance >= 9) { render_char1 = WALL_CHAR_7; render_char2 = WALL_CHAR_6; light = 244;}
    else if (distance >= 7.75) { render_char1 = WALL_CHAR_6; render_char2 = WALL_CHAR_6; light = 245;}
    else if (distance >= 6.5) { render_char1 = WALL_CHAR_6; render_char2 = WALL_CHAR_5; light = 246;}
    else if (distance >= 5.25) { render_char1 = WALL_CHAR_5; render_char2 = WALL_CHAR_5; light = 247;}
    else if (distance >= 4.25) { render_char1 = WALL_CHAR_5; render_char2 = WALL_CHAR_4; light = 248;}
    else if (distance >= 3.25) { render_char1 = WALL_CHAR_4; render_char2 = WALL_CHAR_4; light = 249;}
    else if (distance >= 2.5) { render_char1 = WALL_CHAR_4; render_char2 = WALL_CHAR_3; light = 250;}
    else if (distance >= 1.75) { render_char1 = WALL_CHAR_3; render_char2 = WALL_CHAR_3; light = 251;}
    else if (distance >= 1.25) { render_char1 = WALL_CHAR_3; render_char2 = WALL_CHAR_2; light = 252;}
    else if (distance >= 0.75) { render_char1 = WALL_CHAR_2; render_char2 = WALL_CHAR_2; light = 253;}
    else if (distance >= 0.5) { render_char1 = WALL_CHAR_2; render_char2 = WALL_CHAR_1; light = 254;}
    else if (distance >= 0.25) { render_char1 = WALL_CHAR_1; render_char2 = WALL_CHAR_1; light = 255;}
    
    
    else { render_char1 = WALL_CHAR_BLANK; render_char2 = WALL_CHAR_BLANK; light = 255; }
    int index = 0;
    for (int i = 0; i < draw_start; ++i) {
        index = pixel_to_index(column, i);
        sprintf(&buffer[index], "\033[48;5;%03dm\033[38;5;%03dm%c", 232, 255, ' ');
    }
    int light_offset = 0;
    for (int i = draw_start; i <= draw_end; i += 2) {
        index = pixel_to_index(column, i);

        sprintf(&buffer[index], "\033[48;5;%03dm\033[38;5;%03dm%c", 232, light, render_char1);

        if (i + 1 < SCREEN_HEIGHT) {
            index = pixel_to_index(column, i + 1);
            sprintf(&buffer[index], "\033[48;5;%03dm\033[38;5;%03dm%c", 232, light, render_char2);
            
        }        
    }

    for (int i = draw_end + 2; i < SCREEN_HEIGHT; ++i) {
        index = pixel_to_index(column, i);
        sprintf(&buffer[index], "\033[48;5;%03dm\033[38;5;%03dm%c", 235, 255, ' ');
    }
}

void render_screen(struct camera * cam, struct screen * scr, int grid[]) {
    float camera_x;
    float aspect_ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    float ray_dir_x;
    float ray_dir_y;
    int grid_index = 0;
    float distance = 0;
    float perspective_corrected_distance = 0;
    int added = 0;

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
       
        get_ray_dir(cam, &ray_dir_x, &ray_dir_y, x);

        distance = cast_ray(cam->pos_x, cam->pos_y, ray_dir_x, ray_dir_y, grid, &grid_index);
        perspective_corrected_distance = distance * 
            cos(atan2(ray_dir_y, ray_dir_x) - cam->angle);
        render_column(scr->buffer[scr->current_buffer], x, perspective_corrected_distance);
    }
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        scr->buffer[scr->current_buffer][pixel_to_index(SCREEN_WIDTH, y)] = '\n';
    }
    
    scr->buffer[scr->current_buffer][BUFFER_SIZE] = '\0';
}

void get_ray_dir(struct camera * cam, float * ray_dir_x, float * ray_dir_y, int column) {
    float camera_x;
    float aspect_ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
        camera_x = (2 * (float)column / (float)SCREEN_WIDTH) - 1;
        float dir_x = cos(cam->angle);
        float dir_y = sin(cam->angle);

        float plane_x = (-sin(cam->angle) * aspect_ratio) * FOV;
        float plane_y = (cos(cam->angle) * aspect_ratio) * FOV;

        *ray_dir_x = dir_x + plane_x * camera_x;
        *ray_dir_y = dir_y + plane_y * camera_x;

        float length = sqrt(*ray_dir_x * *ray_dir_x + *ray_dir_y * *ray_dir_y);
        *ray_dir_x /= length;
        *ray_dir_y /= length;
}

/*
    newlines 0 if you want access to grid element, 
    1 for index to char[] that has one newline per y
*/
int grid_to_index(int x, int y, int newlines) {
    return ((GRID_X) + newlines) * (y) + x;
}

int pixel_to_index(int x, int y) {
    return (SCREEN_WIDTH * (WIDTH_ESCAPE + 1) + 1) * y + x * (WIDTH_ESCAPE + 1);
}

void get_grid(int grid[], int selection) {
 int g[30 * 30] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
        1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    int g2[100] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
      };

    

    
    for (int i = 0; i < GRID_X * GRID_Y; ++i) {
        grid[i] = g[i];
    }
}

/*
    no more than 999
*/
void int_to_char_code(int index, char * hundred, char * ten, char * one) {
    *hundred = index / 100;
    *ten = (index - (*hundred * 100)) / 10;
    *one = (index - (*hundred * 100)) - (*ten * 10);

    *hundred -= '0';
    *ten -= '0';
    *one -= '0';
}