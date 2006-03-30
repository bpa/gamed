import socket
from struct import pack, unpack
from exceptions import RuntimeError
from observing import Observable

COUNTRIES = [ 'EASTERN US', 'NORTHWEST TERRITORY', 'WESTERN US', 'ONTARIO',
  'CENTRAL AMERICA', 'ALBERTA', 'GREENLAND', 'ALASKA', 'QUEBEC', 'BRAZIL',
  'VENEZUELA', 'ARGENTINA', 'PERU', 'ICELAND', 'SOUTHERN EUROPE', 'UKRAINE',
  'SCANDINAVIA', 'GREAT BRITAIN', 'WESTERN EUROPE', 'NORTHERN EUROPE', 'EGYPT',
  'CONGO', 'MADAGASCAR', 'SOUTH AFRICA', 'EAST AFRICA', 'NORTH AFRICA',
  'AFGHANISTAN', 'MONGOLIA', 'URAL', 'JAPAN', 'IRKUTSK', 'INDIA', 'SIAM',
  'YAKUTSK', 'SIBERIA', 'CHINA', 'KAMCHATKA', 'MIDDLE EAST', 'NEW GUINEA',
  'INDONESIA', 'WESTERN AUSTRALIA', 'EASTERN AUSTRALIA',
]

ERRORS = ['Invalid command','Not enough players', 'Not enough armies',
  'Do not own country', 'Invalid destination']

COMMANDS = [ 'NOP', 'PLAYER_JOIN', 'MESSAGE', 'ERROR', 'READY', 'NOTREADY',
  'START_PLACING', 'BEGIN', 'MOVE', 'ATTACK', 'PLACE', 'GET_ARMIES',
  'ATTACK_RESULT', 'MOVE_RESULT', 'GAME_STATUS', 'PLAYER_STATUS',
  'COUNTRY_STATUS', 'DEFEAT', 'VICTORY' ]

def cmd_cmd(cmd):    return ord(cmd[0])
def cmd_from(cmd):   return ord(cmd[1])
def cmd_to(cmd):     return ord(cmd[2])
def cmd_armies(cmd): return ord(cmd[3])

class Client(Observable):
    def __init__(self, host='localhost', port=7483):
        self.sock = socket.socket()
        self.sock.connect((host, port))
        self.country_map = {}
        self.command_map = {}
        for c in range(len(COUNTRIES)):
            self.country_map[COUNTRIES[c]] = c
        for c in range(len(COMMANDS)):
            self.command_map[COMMANDS[c]] = c
        self.countries = []
        for c in range(42):
            self.countries.append(Country(pack('>4b',c,0,0,0)))
        cmd = self.sock.recv(4)
        self.player_id = cmd_from(cmd)
        self.players = self.player_id + 1
        self.reserve = 0
        self.state = "Waiting for players"
        self.sock.setblocking(0)

    def send_command(self, command, f=0, t=0, armies=0):
        command = command.replace(' ', '_').upper()
        if (self.command_map.has_key(command)):
            cmd = self.command_map[command]
            print command
        else:
            raise RuntimeError("Unrecognized command: %s" % command)
        packet = pack(">4b", cmd, f, t, armies)
        self.sock.send(packet, 4)

    def recv_command(self):
        try:
            cmd = self.sock.recv(4)
        except:
            return (False,None)
        c = cmd_cmd(cmd)
        if (c == self.command_map['ERROR']):
            print ERRORS[ord(cmd[1])]
            return (True, None)
        elif (c == 12 or c == 13):
            self.sock.setblocking(1)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == self.command_map['GET_ARMIES']):
            self.reserve = cmd_armies(cmd)
            return (True, 'GET_ARMIES')
        elif (c == self.command_map['COUNTRY_STATUS']):
            self.sock.setblocking(1)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == self.command_map['GAME_STATUS']):
            lands = self.countries
            self.sock.setblocking(1)
            for i in range(42):
                msg = self.sock.recv(4)
                self.countries[ord(msg[0])].update(msg)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == self.command_map['PLAYER_JOIN']):
            self.players = self.players + 1
            return (True, 'PLAYER_JOIN')
        elif (c == self.command_map['START_PLACING']):
            self.state = "Placing Armies"
        elif (c == self.command_map['BEGIN']):
            self.state = "At war"
        elif (c == self.command_map['VICTORY']):
            self.state = "Game Over"
        else:
            return (True, COMMANDS[c])

class Country(Observable):
    def __init__(self, cmd):
        (self.id, self.owner, self.armies) = unpack(">BBBx", cmd)

    def update(self, cmd):
        (self.id, self.owner, self.armies) = unpack(">BBBx", cmd)

    def __repr__(self):
        return "[%15s: %i]" % (color(self.owner + 31, COUNTRIES[self.id]), self.armies)

def color(code, str):
    return "%c[%im%s%c[0m" % (27, code, str, 27)
