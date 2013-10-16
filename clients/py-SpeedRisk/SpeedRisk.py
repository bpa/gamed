from Client import Client
from struct import pack, unpack
import yaml

class Player:
    def __init__(self, id):
        self.id = id
        self.name = None
        self.ready = False
        self.active = False

class Country:
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.owner = -1
        self.armies = 0

class SpeedRisk(Client):
    def __init__(self, host, port, board, name):
        Client.__init__(self, host, port, name)
        self.init_board(board)
        self.reserve = 0
        self.player_id = 0

    def on_join_game(self, game):
        self.player_status()

    def on_create_game(self, game):
        self.player_status()

    def on_player_join(self):
        self.list_players()

    def on_player_quit(self):
        self.list_players()

    def on_player_status(self, f, t, a):
        self.player_id = f

    def init_board(self, board_name):
        with open("resources/%s/board.yml" % board_name) as f:
            self.board = yaml.load(f)
        self.players = map(lambda x: Player(x), range(self.board['players']))
        c = 0
        self.countries = {}
        for t in self.board['territories']:
            self.countries[t['name']] = c
            c = c + 1
        self.borders = []
        for t in enumerate(self.board['territories']):
            pass

    def borders(self, a, b):
        try:
            return self.board['territories'][a][b]
        except:
            return False
        
    def on_game_message(self, msg):
        data = None
        if len(msg) > 4:
            (op, f, t, a, data) = unpack(">4b%is" % (len(msg) - 4,), msg)
        else:
            (op, f, t, a) = unpack(">4b", msg)
        try:
            name = SpeedRisk._commands[op]
        except AttributeError:
            print "No command known for op", op
            return
        print name, f, t, a
        try:
            handler = getattr(self, name)
            args = handler.func_code.co_argcount
            if args == 1:
                handler()
            elif args == 2:
                handler(data)
            else:
                if data == None:
                    handler(f, t, a)
                else:
                    handler(f, t, a, data)
        except AttributeError:
            if Client.warnings:
                print "No handler for", name

def __command(op, name):
    def wrapped(self, f=0, t=0, a=0):
        print "%s %i => %i %i" % (name, f, t, a)
        self.sock.send(pack(">2BH4B", 5, 6, 4, op, f, t, a))
    setattr(SpeedRisk, name, wrapped)
    SpeedRisk._commands[op] = "on_" + name

__command( 0, 'player_join')
__command( 1, 'message')
__command( 2, 'error')
__command( 3, 'ready')
__command( 4, 'not_ready')
__command( 5, 'start_placing')
__command( 6, 'begin')
__command( 7, 'move')
__command( 8, 'attack')
__command( 9, 'place')
__command(10, 'get_armies')
__command(11, 'attack_result')
__command(12, 'move_result')
__command(13, 'game_status')
__command(14, 'player_status')
__command(15, 'country_status')
__command(16, 'defeat')
__command(17, 'victory')
__command(18, 'player_quit')
__command(19, 'next_army_generation')
