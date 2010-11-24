class Client {
    use Gamed::Role::Client;
    does Gamed::Role::Client;
    has Str @!messages;

    method send ( $message ) {
        @!messages.push(\$message);
    }

    method next () {
        return @!messages.shift;
    }
}
