/* ant.c - Langton's ant cellular automaton.
 * Build: gcc ant.c -o ant */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define W 80
#define H 40

static volatile int running = 1;
static void stop(int s){ (void)s; running = 0; }

int main(void){
    signal(SIGINT, stop);
    static int grid[H][W];
    memset(grid,0,sizeof(grid));
    int ax=W/2, ay=H/2, dir=0;        /* 0=up,1=right,2=down,3=left */
    int dx[4]={0,1,0,-1}, dy[4]={-1,0,1,0};
    long steps=0;
    printf("\033[2J\033[?25l");
    while (running){
        for (int k=0;k<150 && running;k++){
            if (grid[ay][ax]){ dir=(dir+3)%4; grid[ay][ax]=0; }
            else { dir=(dir+1)%4; grid[ay][ax]=1; }
            ax=(ax+dx[dir]+W)%W; ay=(ay+dy[dir]+H)%H;
            steps++;
        }
        printf("\033[H");
        for (int y=0;y<H;y++){
            for (int x=0;x<W;x++){
                if (x==ax&&y==ay) putchar('@');
                else putchar(grid[y][x]?'#':' ');
            }
            putchar('\n');
        }
        printf("steps: %ld   (Ctrl-C to quit)", steps);
        fflush(stdout);
        usleep(20000);
    }
    printf("\033[0m\033[?25h\033[2J\033[H");
    return 0;
}
