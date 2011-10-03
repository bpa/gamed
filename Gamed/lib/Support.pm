module Support;

use NativeCall;

sub sha1sum(Str $data) returns Buf is export is native('support') { ... }
sub sha1_base64(Str $data) returns Str is export is native('support') { ... }
sub base64_encode(Str $data) returns Str is export is native('support') { ... }
