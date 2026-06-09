# Langton's Ant in C

A Turing-complete cellular automaton — Langton's Ant — rendered at true color via SDL2 on a toroidal grid.

The ant follows two simple rules (turn right on white, turn left on black, flip the cell, step forward) yet produces emergent chaotic growth that eventually self-organizes into a repeating "highway" pattern after ~10,000 steps.

---

# Features

- Turing-complete four-rule cellular automaton
- Emergent self-organization from chaotic to periodic behavior
- Seamless toroidal wraparound on all four edges
- True 24‑bit color pixel rendering via SDL2 framebuffer
- 120 × 80 grid at 8 px per cell — 960 × 640 window
- 200 simulation steps per frame at ~60 fps
- Live step counter in window title
- Keyboard quit handling (ESC / Q)
- Written entirely in C

---

# How It Works

A virtual "ant" walks a 2‑D grid of cells. Each cell is either white or black. The ant reads the color of the cell it stands on, turns accordingly, flips the cell's color, and steps forward. Despite the simplicity of the rules, the ant's trajectory is chaotic for thousands of steps before spontaneously locking into a repeating diagonal highway.

Each frame batches 200 simulation ticks, then redraws every cell as a colored square in an SDL2 pixel buffer. The ant is rendered as a red 5×5 dot on top.

---

# Tutorial / Cellular Automaton Breakdown

## 1. The Grid

The world is a fixed-size 2‑D integer array where `0` = white and `1` = black:

```c
#define GRID_W    120
#define GRID_H    80

static int grid[GRID_H][GRID_W];
memset(grid, 0, sizeof(grid));
```

All cells start white. The grid is **toroidal** — stepping off one edge wraps around to the opposite side.

---

## 2. The Ant State

The ant is three variables: its column `ax`, its row `ay`, and a direction `dir` encoded as `0` = up, `1` = right, `2` = down, `3` = left:

```c
int ax = GRID_W / 2, ay = GRID_H / 2, dir = 0;

int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { -1, 0, 1, 0 };
```

Indexing `dx[dir]` and `dy[dir]` gives the column and row delta for one forward step.

---

## 3. The Four Rules

Every tick the ant reads the current cell, turns, flips it, and advances:

```c
if (grid[ay][ax]) {          /* black cell */
    dir = (dir + 3) & 3;     /* turn left  (three right-turns ≡ left) */
    grid[ay][ax] = 0;        /* flip to white */
} else {                      /* white cell */
    dir = (dir + 1) & 3;     /* turn right */
    grid[ay][ax] = 1;        /* flip to black */
}

ax = (ax + dx[dir] + GRID_W) % GRID_W;   /* step with toroidal wrap */
ay = (ay + dy[dir] + GRID_H) % GRID_H;
steps++;
```

`(dir + 1) & 3` rotates clockwise; `(dir + 3) & 3` rotates counter‑clockwise (bitwise‑and with 3 is equivalent to `% 4` but faster). The `+ GRID_W` / `+ GRID_H` before the modulo guarantees a non‑negative result when walking off a negative edge.

---

## 4. Batching for Smooth Animation

Instead of redrawing after every tick, the inner loop accumulates 200 steps before each frame:

```c
for (int k = 0; k < 200 && running; k++) {
    /* rules from step 3 */
}
```

200 ticks per frame at a 16 ms cap gives ~12,500 simulation steps per second while keeping the display smooth at ~60 fps.

---

## 5. SDL2 Pixel-Buffer Rendering

Each grid cell is drawn as an `CELL × CELL` (8 × 8) pixel block. The surface is written directly:

```c
for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
        Uint32 color = grid[y][x] ? col_black : col_white;
        for (int py = 0; py < CELL; py++)
            for (int px = 0; px < CELL; px++)
                pixels[(y * CELL + py) * pitch + (x * CELL + px)] = color;
    }
}
```

The ant is overlaid as a 5×5 red dot at its current grid position:

```c
int ant_cx = ax * CELL + CELL / 2;
int ant_cy = ay * CELL + CELL / 2;
for (int py = -2; py <= 2; py++)
    for (int px = -2; px <= 2; px++)
        if (sx in bounds) pixels[sy * pitch + sx] = col_ant;
```

This gives **true 24‑bit color** — no ASCII quantization, no palette limits.

---

## 6. Color Palette

Three pre-computed colors define the visual:

| Element  | RGB         | Role          |
|----------|-------------|---------------|
| White    | 240, 230, 210 | Warm paper background |
| Black    |  30,  30,  38 | Near‑black slate   |
| Ant      | 220,  50,  50 | Bright red marker  |

---

## 7. Frame Timing and Window Title

A 60 fps cap prevents busy-waiting:

```c
Uint32 elapsed = SDL_GetTicks() - frame_start;
if (elapsed < 16) SDL_Delay(16 - elapsed);
```

The window title updates every frame with the cumulative step count:

```c
snprintf(title, sizeof title, "Langton's Ant — %ld steps", steps);
SDL_SetWindowTitle(win, title);
```

---

# Build

```
git clone <this-repo>
cd ant
make
```

Or manually:

```
gcc ant.c -o ant $(sdl2-config --cflags --libs)
```

**Dependencies:** SDL2 and a C compiler (`gcc`/`clang`).

Install SDL2 via Homebrew:

```
brew install sdl2
```

Or apt:

```
sudo apt install libsdl2-dev
```

---

# Run

```
./ant
```

Controls:
- **ESC** or **Q** — quit

The ant starts at the center of a 120 × 80 toroidal grid. After a chaotic phase of ~10,000 steps it spontaneously forms a diagonal highway.

---

# Customizing

Edit the constants at the top of `ant.c`:

- `GRID_W`, `GRID_H` — grid dimensions (larger = more patterns visible)
- `CELL` — pixel size per grid cell (larger = blockier look)
- Color values (`col_white`, `col_black`, `col_ant`) — any RGB triple
- Batch size (the `200` in the inner loop) — higher = faster simulation

---

# Concepts Practiced

- Turing-complete cellular automata
- Langton's Ant four-rule state machine
- Emergent self-organization (chaos → highway)
- Toroidal grid with modular wraparound
- Direction encoding via lookup tables
- SDL2 surface/pixel-buffer graphics
- True-color 24-bit framebuffer rendering
- Real-time animation with batched simulation steps
- Event-driven windowing and keyboard input
- Frame-rate capping with adaptive delay

---

# Dependencies

- `SDL.h` — window management and pixel-buffer rendering
- `stdio.h` — `stderr` diagnostics, `snprintf`
- `stdlib.h` — `NULL`, `EXIT_*` macros
- `string.h` — `memset`
