# Langton's Ant

Langton's ant cellular automaton, in C.

A small, self-contained demo written in **pure C** — no external libraries,
just the standard library and POSIX. Part of the [Corg-Labs](https://github.com/Corg-Labs)
collection of single-file C programs.

---

## How It Works

1. The ant turns right on a white cell, left on a black cell
2. It flips the cell's color, then steps forward
3. Order emerges into a 'highway' after ~10,000 steps
4. A live counter tracks the number of steps taken

---

# Tutorial

This walkthrough explains every moving part of `ant.c` — from the grid and
direction tables right through to the terminal-rendering loop.

## 1. The Grid and Its Constants

The entire playing field is a single 2-D integer array where `0` means white
and `1` means black:

```c
#define W 80
#define H 40

static int grid[H][W];
memset(grid, 0, sizeof(grid));
```

`W` and `H` set the terminal dimensions. `memset` zeroes the whole grid so
every cell starts white.

## 2. Ant State and Direction Encoding

The ant is represented by three variables: its column `ax`, its row `ay`, and a
facing direction `dir` encoded as an integer (0 = up, 1 = right, 2 = down,
3 = left):

```c
int ax = W/2, ay = H/2, dir = 0;   /* 0=up,1=right,2=down,3=left */
int dx[4] = {0, 1, 0, -1};
int dy[4] = {-1, 0, 1, 0};
```

`dx` and `dy` are parallel look-up tables: index them with `dir` to get the
column and row delta for a single forward step.

## 3. Applying the Langton's Ant Rules

Each tick the ant examines the cell it currently stands on, turns, flips the
cell, and moves forward:

```c
if (grid[ay][ax]) { dir = (dir+3)%4; grid[ay][ax] = 0; }
else              { dir = (dir+1)%4; grid[ay][ax] = 1; }
ax = (ax + dx[dir] + W) % W;
ay = (ay + dy[dir] + H) % H;
steps++;
```

`(dir+1)%4` turns right; `(dir+3)%4` is equivalent to turning left (three
right-turns). The `+W`/`+H` trick before the modulo keeps the result positive
even when the ant walks off a negative edge, giving seamless wraparound on all
four sides.

## 4. Batching Steps for Smooth Animation

Rather than redrawing after every single step (which would make the display
flicker and stall), the inner loop advances the simulation 150 ticks before
each render:

```c
for (int k = 0; k < 150 && running; k++) {
    /* ... rules from step 3 ... */
}
```

150 iterations per frame at a 20 ms sleep (`usleep(20000)`) gives ~7,500
simulation steps per second while keeping CPU usage low.

## 5. Terminal Rendering with ANSI Escape Codes

Before the main loop the cursor is hidden and the screen cleared:

```c
printf("\033[2J\033[?25l");
```

At the start of every frame the cursor jumps back to the top-left corner
instead of clearing the screen again — that prevents flicker:

```c
printf("\033[H");
```

Each cell is then printed as one character: `'@'` for the ant's current
position, `'#'` for a black cell, and `' '` (space) for a white cell:

```c
if (x == ax && y == ay) putchar('@');
else putchar(grid[y][x] ? '#' : ' ');
```

## 6. The Step Counter and Graceful Shutdown

A `long` counter accumulates the total number of rule applications and is
displayed below the grid each frame:

```c
printf("steps: %ld   (Ctrl-C to quit)", steps);
```

A `SIGINT` handler (Ctrl-C) flips a `volatile` flag so both the inner and
outer loops exit cleanly:

```c
static volatile int running = 1;
static void stop(int s) { (void)s; running = 0; }
```

On exit, the terminal is restored — SGR reset, cursor re-enabled, screen
cleared:

```c
printf("\033[0m\033[?25h\033[2J\033[H");
```

---

## Build

```
gcc ant.c -o ant
```

## Run

```
./ant
```

## Controls

Press **Ctrl-C** to quit.
