#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include "raycast.h"

void enable_non_blocking_input() {
    struct termios t;

    // Get terminal settings
    tcgetattr(STDIN_FILENO, &t);

    // Turn off canonical mode and echo
    t.c_lflag &= ~(ICANON | ECHO);

    // Apply settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // Set stdin to non-blocking
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disable_non_blocking_input() {
    struct termios t;

    // Get current settings
    tcgetattr(STDIN_FILENO, &t);

    // Restore canonical mode and echo
    t.c_lflag |= ICANON | ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // Reset to blocking mode
    fcntl(STDIN_FILENO, F_SETFL, 0);
}

int main() {
    enable_non_blocking_input();
    setvbuf(stdout, NULL, _IONBF, 0);  // Disable stdout buffering entirely

    clock_t start_time = clock();
    struct screen scr;
    scr.current_buffer = 0;
    scr.buffer[0] =  malloc(BUFFER_SIZE * sizeof(char) + 1);
    if (scr.buffer[0] == NULL) {
        perror("malloc failed");
        return 1;
    }
    scr.buffer[1] =  malloc(BUFFER_SIZE * sizeof(char) + 1);
    if (scr.buffer[1] == NULL) {
        perror("malloc failed");
        return 1;
    }
    //char map[(GRID_X + SCREEN_WIDTH + 1) * GRID_Y + 1];

    struct camera cam;
    cam.pos_x = 2;
    cam.pos_y = 2;
    cam.walk_speed = 0.001;
    cam.turn_speed = 0.001;
    cam.angle = 3;
    cam.turning = 0;
    cam.walking = 0;
    float dir_x = 0;
    float dir_y = 0;
    float ray_dir_x = 0;
    float ray_dir_y = 0;

    int grid[GRID_X * GRID_Y];
    get_grid(grid, 1);
    int grid_index = 0;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000 * 1000; // 1 ms
       
                    
    char ch;
    int running = 1;
    clear_buffer(&scr);
    while (running) {
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            //printf("You pressed: %c\n", ch);
            if (ch == 'q') break;
            if (ch == 'w') { cam.walking = 1; cam.turning = 0; }
            else if (ch == 's') { cam.walking = -1; cam.turning = 0; }
            else if (ch == 'a') cam.turning = -1;
            else if (ch == 'd') cam.turning = 1;
            else if (ch == ' ') { cam.walking = 0; cam.turning = 0; }
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("read");
        }

        cam.angle += cam.turn_speed * cam.turning;
        dir_x = cos(cam.angle);
        dir_y = sin(cam.angle);
        get_ray_dir(&cam, &dir_x, &dir_y, SCREEN_WIDTH / 2 + 1);

        if (cast_ray(cam.pos_x, cam.pos_y, 
            dir_x, dir_y, grid, &grid_index) > WALL_BLOCK_DISTANCE) {
            
            cam.pos_x += dir_x * cam.walk_speed * cam.walking;
            cam.pos_y += dir_y * cam.walk_speed * cam.walking;
        } else {
            cam.pos_x += dir_x * cam.walk_speed * -cam.walking;
            cam.pos_y += dir_y * cam.walk_speed * -cam.walking;
        }
        
        
        //Render and print the screen
        //clear_buffer(&scr);
        render_screen(&cam, &scr, grid);
        //render map
        int index_compare = 0;
        int index_char = 0;
        int map_x_pos = 0;
        int map_y_pos = 0;
        //int newline_index = map_x_pos - SCREEN_WIDTH;

         
        for (int i = 0; i < GRID_Y; ++i) {
            for (int j = 0; j < GRID_X; ++j) {
                index_compare = grid_to_index(j, i, 0);
                index_char = grid_to_index(j, i, 1);
                if (index_compare == grid_index) {
                    index_char = '@';
                } else if (j == (int)cam.pos_x && i == (int)cam.pos_y) {
                    index_char = 'P'; 

                } else if (grid[index_compare]) {
                    index_char = '+';
                } else index_char = ' ';
                scr.buffer[scr.current_buffer][pixel_to_index(map_x_pos + j, map_y_pos + i) + WIDTH_ESCAPE] = index_char;

            }
            //newline_index = grid_to_index(GRID_X, i, 1);
            //map[newline_index] = '\n';
            
               
        }
        //map[(GRID_X + 1) * GRID_Y] = '\0';

        printf("\033[H");
        printf("%s", scr.buffer[scr.current_buffer]);
        //printf("\033[0;%dH", 0);// + (SCREEN_WIDTH / 4));
        //printf("%s", map);
        //fflush(stdout);


        // Do other stuff here (game loop, updates, etc.)
        while ((double)(clock() - start_time) / (double)CLOCKS_PER_SEC < 0.01667) {
            nanosleep(&ts, NULL);
        }
        //nanosleep(&ts, NULL); // sleep for 10 ms to avoid high CPU usage
    }

    disable_non_blocking_input();

    printf("\n\033[38;5;255mGGGGGG\033[38;5;245mGGGGGG\033[38;5;235mGGGGGG");
    printf("\n\033[38;5;245mGGGGGG");
    printf("\n\033[38;5;235mGGGGGG");
    printf("\n\033[38;5;232mGGGGGG");
    printf("\n\033[38;5;236mGGGGGG");

    printf("\n\033[38;5;237mGGGGGG");

    printf("\n\033[38;5;238mGGGGGG");

    return 0;
}

