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
