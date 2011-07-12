class ncurses_client;

use ncurses;

has $!screen;
has $!input_window;
has $!output_window;

submethod BUILD() {
my $scr = initscr();
keypad($scr, 1);
noecho();
cbreak();
start_color();
use_default_colors();
curs_set(0);
my $win = newwin(0, 0, 0, 0);
my $x = getmaxx($scr);
my $y = getmaxy($scr);
my $log = subwin($win, 0, $x-4, 0, 0);
scrollok($log, 1);
winsertln($log);
waddstr($log, "Hello");
mvwhline($win, $y-4, 0, 0, $x);
my $input = newwin(3, 0, $y-3, 0);
wrefresh($win);
loop {
	my $c = getch();
    last if $c ~~ ord('q');
	waddstr($input, $c);
	wrefresh($input);
}
delwin($log);
delwin($input);
delwin($win);
endwin();
}

