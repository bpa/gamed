from SpeedRisk import SpeedRisk
from time import sleep
import sys
import Client

class myclient(SpeedRisk):
    def __init__(self, name, port):
        SpeedRisk.__init__(self, "localhost", port, "ClassicRisk", name)

    def on_list_game_instances(self, message):
        print message
        if message == '0':
            self.create_game("SpeedRisk:test")
        else:
            self.join_game("SpeedRisk:"+message.split(':')[1])

    def on_list_players(self, message):
        print "Players:", self.deserialize(message)
        self.list_themes()

    def on_get_armies(self):
        self.ready()

    def on_ready(self, f, t, a):
        if f != self.player_id:
            self.ready()

    def on_player_quit(self):
        self.running = False

    def on_list_themes(self, message):
        print message

    def run(self):
        self.running = True
        self.list_game_instances('SpeedRisk')
        while self.running:
            sleep(.1)
            self.poll()
        self.quit_game()

if __name__ == '__main__':
    c = myclient(sys.argv[1], int(sys.argv[2]))
    c.run()
