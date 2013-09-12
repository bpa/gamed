from client.SpeedRisk import SpeedRisk
from time import sleep
import sys

class myclient(SpeedRisk):
    def __init__(self, name):
        SpeedRisk.__init__(self, "localhost", 7483, name)

    def on_list_game_instances(self, message):
        print message
        if message == '0':
            self.create_game("SpeedRisk:test")
        else:
            self.join_game("SpeedRisk:"+message.split(':')[1])

    def on_list_players(self, message):
        print "Players:", message

    def on_get_armies(self):
        self.ready()

    def on_ready(self, f, t, a):
        if f != self.player_id:
            self.ready()

    def on_player_quit(self):
        self.running = False

    def run(self):
        self.running = True
        c.list_game_instances('SpeedRisk')
        while self.running:
            sleep(.1)
            c.poll()
        c.quit_game()

if __name__ == '__main__':
    c = myclient(sys.argv[1])
    c.run()
