role Gamed::Role::Game {
    #use Gamed::Role::Client;
    #does Gamed::Role::Plugin;
    #requires $.version;

    has $.max_players = 0;
    has @.clients;

#    method announce ( Gamed::Role::Message $message ) {
#        .send($message) for @.clients;
#    }
#
#    method is_join_allowed ( Gamed::Role::Client $client ) {
#        return 1 unless $.max_players;
#        return 1 if @.clients < $.max_players;
#        $client.send( [ game => 'join' => error => 'Game is full' ] );
#        return;
#    }
#
#    submethod add ( $client ) {
#        @.clients.push($client);
#//        $client.game = $self;
#    }

}

=begin END
#head1 METHODS
#
#item add
#      Adds a client to the game.  Will call join_allowed first
#item announce
#      Send a message to all players in the game
#item is_join_allowed $client
#      Whether or not a client should be allowed to join.
#back
