role Gamed::Role::Command {
    use Gamed::Role::Plugin;
    does Gamed::Role::Plugin;
    #requires &.handle;
}
