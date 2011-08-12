use Commands;

class Commands::Server does Commands;

method cmd_create($name, $type, :$cmd) {
	$game = client($type);
	$conn.give({s=>'create',name=>$name,game=>$type});
}

method cmd_join($name, :$cmd) {
	$game = client($name);
	$conn.give({s=>'join',name=>$name});
}

method cmd_quit(:$cmd) {
	$conn.give({ s => 'quit game' });
}

method cmd_exit(:$cmd) {
	exit 0;
}

