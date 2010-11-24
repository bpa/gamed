role Gamed::Role::Pluggable {
    method plugins ( Str $role, Str $lib ) {
        #$my $p = Module::Pluggable::Fast.new;
        #$p.search = $lib;
        #$p.callback = sub ( $class ) { $class.use; my $obj = eval($class).new; say "Obj: {$obj.WHAT}"};
        #$p.require = 1;

        my %hash;
        #for $p.plugins -> $class {
            #try {
                #eval("use $class;");
                #my $obj = $class.eval.new;
                #if $obj.does($role) {
                    #%hash{$obj.name} = $obj;
                #}
            #}
        #}
        return \%hash;
    }
}
