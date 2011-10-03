use Test;
use Support;

is base64_encode('e'),    'ZQ==', 'base64_encode: e';
is base64_encode('en'),   'ZW4=', 'base64_encode: en';
is base64_encode('enc'),  'ZW5j', 'base64_encode: enc';

is sha1_base64('e'),   'WOazpBSh4JDfxgKa3Q81Vcy6En8=', 'sha1_base64: e';
is sha1_base64('en'),  'CUsP4OMChUrxMRr6uFtSA7pFejs=', 'sha1_base64: en';
is sha1_base64('enc'), 'J3/XZFaIBDdkH3beG/ptfvYa6GE=', 'sha1_base64: enc';
