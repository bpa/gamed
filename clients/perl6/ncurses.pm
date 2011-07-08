module ncurses;

use NativeCall;

sub initscr() returns OpaquePointer is export is native('libncurses') { ... }
sub endwin() returns Int is export is native('libncurses') { ... }
sub isendwin() returns Int is export is native('libncurses') { ... }
sub newterm(Str $type, Int $outfd, Int $infd) returns OpaquePointer is export is native('libncurses') { ... }
sub set_term(OpaquePointer $screen) returns OpaquePointer is export is native('libncurses') { ... }
sub delscreen(OpaquePointer $screen) is export is native('libncurses') { ... }
sub addstr(Str $str) returns Int is export is native('libncurses') { ... }
sub addnstr(Str $str, Int $n) returns Int is export is native('libncurses') { ... }
sub waddstr(OpaquePointer $win, Str $str) returns Int is export is native('libncurses') { ... }
sub waddnstr(OpaquePointer $win, Str $str, Int $n) returns Int is export is native('libncurses') { ... }
sub mvaddstr(Int $y, Int $x, Str $str) returns Int is export is native('libncurses') { ... }
sub mvaddnstr(Int $y, Int $x, Str $str, Int $n) returns Int is export is native('libncurses') { ... }
sub mvwaddstr(OpaquePointer $win, Int $y, Int $x, Str $str) returns Int is export is native('libncurses') { ... }
sub mvwaddnstr(OpaquePointer $win, Int $y, Int $x, Str $str, Int $n) returns Int is export is native('libncurses') { ... }
sub refresh() returns Int is export is native('libncurses') { ... }
sub wrefresh(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub wnoutrefresh(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub doupdate() returns Int is export is native('libncurses') { ... }
sub redrawwin(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub wredrawln(OpaquePointer $win, Int $beg_line, Int $num_lines) returns Int is export is native('libncurses') { ... }
sub newwin(Int $nlines, Int $ncols, Int $begin_y, Int $begin_x) returns OpaquePointer is export is native('libncurses') { ... }
sub delwin(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub mvwin(OpaquePointer $win, Int $y, Int $x) returns Int is export is native('libncurses') { ... }
sub subwin(OpaquePointer $orig, Int $nlines, Int $ncols, Int $begin_y, Int $begin_x) returns OpaquePointer is export is native('libncurses') { ... }
sub derwin(OpaquePointer $orig, Int $nlines, Int $ncols, Int $begin_y, Int $begin_x) returns OpaquePointer is export is native('libncurses') { ... }
sub mvderwin(OpaquePointer $win, Int $par_y, Int $par_x) returns Int is export is native('libncurses') { ... }
sub dupwin(OpaquePointer $win) returns OpaquePointer is export is native('libncurses') { ... }
sub wsyncup(OpaquePointer $win) is export is native('libncurses') { ... }
sub syncok(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub wcursyncup(OpaquePointer $win) is export is native('libncurses') { ... }
sub wsyncdown(OpaquePointer $win) is export is native('libncurses') { ... }
sub getbegx(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getbegy(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getcurx(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getcury(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getmaxx(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getmaxy(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getparx(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub getpary(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub cbreak() returns Int is export is native('libncurses') { ... }
sub nocbreak() returns Int is export is native('libncurses') { ... }
sub echo() returns Int is export is native('libncurses') { ... }
sub noecho() returns Int is export is native('libncurses') { ... }
sub halfdelay(Int $tenths) returns Int is export is native('libncurses') { ... }
sub intrflush(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub keypad(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub meta(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub nodelay(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub raw() returns Int is export is native('libncurses') { ... }
sub noraw() returns Int is export is native('libncurses') { ... }
sub noqiflush() is export is native('libncurses') { ... }
sub qiflush() is export is native('libncurses') { ... }
sub notimeout(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub timeout(Int $delay) is export is native('libncurses') { ... }
sub wtimeout(OpaquePointer $win, Int $delay) is export is native('libncurses') { ... }
sub typeahead(Int $fd) returns Int is export is native('libncurses') { ... }
sub clearok(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub idlok(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub idcok(OpaquePointer $win, Int $bf) is export is native('libncurses') { ... }
sub immedok(OpaquePointer $win, Int $bf) is export is native('libncurses') { ... }
sub leaveok(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub setscrreg(Int $top, Int $bot) returns Int is export is native('libncurses') { ... }
sub wsetscrreg(OpaquePointer $win, Int $top, Int $bot) returns Int is export is native('libncurses') { ... }
sub scrollok(OpaquePointer $win, Int $bf) returns Int is export is native('libncurses') { ... }
sub nl() returns Int is export is native('libncurses') { ... }
sub nonl() returns Int is export is native('libncurses') { ... }
sub hline(Str $ch, Int $n) returns Int is export is native('libncurses') { ... }
sub whline(OpaquePointer $win, Str $ch, Int $n) returns Int is export is native('libncurses') { ... }
sub mvhline(Int $y, Int $x, Str $ch, Int $n) returns Int is export is native('libncurses') { ... }
sub mvwhline(OpaquePointer $win, Int $y, Int $x, Str $ch, Int $n) returns Int is export is native('libncurses') { ... }
sub start_color() returns Int is export is native('libncurses') { ... }
sub init_pair(Int $pair, Int $f, Int $b) returns Int is export is native('libncurses') { ... }
sub init_color(Int $color, Int $r, Int $g, Int $b) returns Int is export is native('libncurses') { ... }
sub has_colors() returns Int is export is native('libncurses') { ... }
sub can_change_color() returns Int is export is native('libncurses') { ... }
sub use_default_colors() returns Int is export is native('libncurses') { ... }
sub assume_default_colors(Int $fg, Int $bg) returns Int is export is native('libncurses') { ... }
sub deleteln() returns Int is export is native('libncurses') { ... }
sub wdeleteln(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub insdelln(Int $n) returns Int is export is native('libncurses') { ... }
sub winsdelln(OpaquePointer $win, Int $n) returns Int is export is native('libncurses') { ... }
sub insertln() returns Int is export is native('libncurses') { ... }
sub winsertln(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub curs_set(Int $visibility) returns Int is export is native('libncurses') { ... }
sub getch() returns Int is export is native('libncurses') { ... }
sub wgetch(OpaquePointer $win) returns Int is export is native('libncurses') { ... }
sub mvgetch(Int $y, Int $x) returns Int is export is native('libncurses') { ... }
sub mvwgetch(OpaquePointer $win, Int $y, Int $x) returns Int is export is native('libncurses') { ... }
sub ungetch(Int $ch) returns Int is export is native('libncurses') { ... }
sub has_key(Int $ch) returns Int is export is native('libncurses') { ... }
