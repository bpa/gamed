//#include "SpeedRiskClient.h"
//#include <SpeedRisk/game.h>
#include <curses.h>
#include <stdio.h>

int main() {
    initscr();
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Color support not available, exiting...");
        return 1;
    }
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    start_color();
    
    init_pair(1,COLOR_WHITE,COLOR_BLUE);
    bkgd(COLOR_PAIR(1));
    addstr("Hello world!");
    attrset(0);
    addstr(" - Hello");
    while (getch() != 'q');
    use_default_colors();
    refresh();
    endwin();
    return 0;
}

