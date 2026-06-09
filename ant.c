/*
 * ant.c - Langton's Ant cellular automaton (SDL2)
 *
 * A four-rule Turing-complete cellular automaton running on a
 * toroidal grid, rendered at true color via SDL2.
 *
 * Build:
 *   gcc ant.c -o ant $(sdl2-config --cflags --libs)
 * Run:
 *   ./ant
 *
 * Controls:
 *   ESC or Q  - quit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#define GRID_W    120
#define GRID_H    80
#define CELL      8

#define WIN_W (GRID_W * CELL)
#define WIN_H (GRID_H * CELL)

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Langton's Ant",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *surf = SDL_GetWindowSurface(win);
    SDL_PixelFormat *fmt = surf->format;
    int pitch = surf->pitch / 4;

    static int grid[GRID_H][GRID_W];
    memset(grid, 0, sizeof(grid));

    int ax = GRID_W / 2, ay = GRID_H / 2, dir = 0;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    long steps = 0;
    int running = 1;

    Uint32 col_white = SDL_MapRGB(fmt, 240, 230, 210);
    Uint32 col_black = SDL_MapRGB(fmt, 30,  30,  38);
    Uint32 col_ant   = SDL_MapRGB(fmt, 220, 50,  50);

    while (running) {
        Uint32 frame_start = SDL_GetTicks();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                running = 0;
            if (ev.type == SDL_KEYDOWN &&
                (ev.key.keysym.sym == SDLK_ESCAPE ||
                 ev.key.keysym.sym == SDLK_q))
                running = 0;
        }

        /* Batch 200 simulation steps per frame */
        for (int k = 0; k < 200 && running; k++) {
            if (grid[ay][ax])        { dir = (dir + 3) & 3; grid[ay][ax] = 0; }
            else                     { dir = (dir + 1) & 3; grid[ay][ax] = 1; }
            ax = (ax + dx[dir] + GRID_W) % GRID_W;
            ay = (ay + dy[dir] + GRID_H) % GRID_H;
            steps++;
        }

        if (SDL_MUSTLOCK(surf))
            SDL_LockSurface(surf);

        Uint32 *pixels = (Uint32 *)surf->pixels;

        for (int y = 0; y < GRID_H; y++) {
            for (int x = 0; x < GRID_W; x++) {
                Uint32 color = grid[y][x] ? col_black : col_white;
                for (int py = 0; py < CELL; py++)
                    for (int px = 0; px < CELL; px++)
                        pixels[(y * CELL + py) * pitch + (x * CELL + px)] = color;
            }
        }

        /* Draw the ant as a 5x5 pixel dot */
        int ant_cx = ax * CELL + CELL / 2;
        int ant_cy = ay * CELL + CELL / 2;
        for (int py = -2; py <= 2; py++) {
            for (int px = -2; px <= 2; px++) {
                int sx = ant_cx + px, sy = ant_cy + py;
                if (sx >= 0 && sx < WIN_W && sy >= 0 && sy < WIN_H)
                    pixels[sy * pitch + sx] = col_ant;
            }
        }

        if (SDL_MUSTLOCK(surf))
            SDL_UnlockSurface(surf);

        SDL_UpdateWindowSurface(win);

        char title[64];
        snprintf(title, sizeof title, "Langton's Ant — %ld steps", steps);
        SDL_SetWindowTitle(win, title);

        Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < 16)
            SDL_Delay(16 - elapsed);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
