use v6;

use Gamed::Role::Plugin;
role Gamed::Role::State {
    does Gamed::Role::Plugin;
    #requires &.handle;

    method enter { }
    method leave { }

}
