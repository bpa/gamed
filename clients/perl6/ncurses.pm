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


