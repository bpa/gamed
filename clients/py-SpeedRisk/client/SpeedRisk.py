import socket
from Client import Client
from struct import pack, unpack

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
    def __init__(self, host, port, name):
        Client.__init__(self, host, port, name)
        self.init_countries()
        self.ready = False
        self.players = map(lambda x: Player(x), range(6))
        self.reserve = 0
        self.player_id = 0

    def on_player_join(self):
        self.list_players()

    def on_player_quit(self):
        self.list_players()

    def init_countries(self):
        self.countries = map(lambda i: Country(i[0], i[1]),
            enumerate(('EASTERN US', 'NORTHWEST TERRITORY', 'WESTERN US',
                'ONTARIO', 'CENTRAL AMERICA', 'ALBERTA', 'GREENLAND',
                'ALASKA', 'QUEBEC', 'BRAZIL', 'VENEZUELA', 'ARGENTINA',
                'PERU', 'ICELAND', 'SOUTHERN EUROPE', 'UKRAINE',
                'SCANDINAVIA', 'GREAT BRITAIN', 'WESTERN EUROPE',
                'NORTHERN EUROPE', 'EGYPT', 'CONGO', 'MADAGASCAR',
                'SOUTH AFRICA', 'EAST AFRICA', 'NORTH AFRICA',
                'AFGHANISTAN', 'MONGOLIA', 'URAL', 'JAPAN', 'IRKUTSK',
                'INDIA', 'SIAM', 'YAKUTSK', 'SIBERIA', 'CHINA',
                'KAMCHATKA', 'MIDDLE EAST', 'NEW GUINEA', 'INDONESIA',
                'WESTERN AUSTRALIA', 'EASTERN AUSTRALIA')))

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
        try:
            handler = getattr(self, name)
            args = handler.func_code.co_argcount
            if args == 1:
                handler(self)
            elif args == 2:
                handler(self, data)
            else:
                if data == None:
                    handler(self, f, t, a)
                else:
                    handler(self, f, t, a, data)
        except AttributeError:
            print "No handler for", name
            pass

def __command(op, name):
    def wrapped(self, f=0, t=0, a=0):
        print "%s %i => %i %i" % (name, f, t, a)
        self.sock.send(pack(">2BH4B", 5, 6, 4, op, f, t, a))
    setattr(SpeedRisk, name, wrapped)
    SpeedRisk._commands[op] = "on_" + name

__command( 0, 'sr_player_join')
__command( 1, 'message')
__command( 2, 'sr_error')
__command( 3, 'sr_ready')
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
