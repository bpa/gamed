import socket
from struct import pack, unpack
from exceptions import RuntimeError
from observing import Observable
from ConfigParser import RawConfigParser
from Constants import *

def cmd_cmd(cmd):    return ord(cmd[0])
def cmd_from(cmd):   return ord(cmd[1])
def cmd_to(cmd):     return ord(cmd[2])
def cmd_armies(cmd): return ord(cmd[3])

class Client(Observable):
    def __init__(self, ui):
        self.init_countries()
        self.ui = ui
        config = RawConfigParser()
        cnf_file = open("config.ini")
        config.readfp(cnf_file)
        cnf_file.close()
        host = config.get('client','host')
        port = config.getint('client','port')
        name = config.get('player','name')
        self.sock = socket.socket()
        self.sock.connect((host, port))
        self.rename(name)
        self.start_game()
        self.sock.setblocking(0)
        self.ready = False

    def start_game(self):
        num = 0
        while 1:
            res = self.start(CMD_JOIN_GAME, "risk%i"%num)
            if res == 'Join Game': break
            if res == 'No Game':
                res = self.start(CMD_CREATE_GAME, "risk%i"%num)
                if res == 'Create Game': break
                else:
                    print res
                    exit
            print res
            num = num + 1
        print "Send: PLAYER_STATUS"
        self.sock.send(pack(">2BH", PLAYER_STATUS, 0, 0))
        res = self.sock.recv(4)
        (cmd,self.player_id,self.players) = unpack(">3Bx", res) 

    def init_countries(self):
        self.country_map = {}
        self.command_map = {}
        for c in range(len(COUNTRIES)):
            self.country_map[COUNTRIES[c]] = c
        for c in range(len(COMMANDS)):
            self.command_map[COMMANDS[c]] = c
        self.countries = []
        for c in range(42):
            self.countries.append(Country(pack('>4b',c,0,0,0)))
        self.reserve = 0
        self.state = "Waiting for players"

    def rename(self, name):        
        l = len(name)
        print "Send: RENAME", name
        self.sock.send(pack(">2BH%is" % l, CMD_PLAYER, CMD_RENAME, l, name))
    
    def start(self, command, instance):
        game = 'SpeedRisk:'
        g = len(game)
        l = len(instance)
        print "Send: CMD_GAME", command, game, instance
        self.sock.send(pack(">2BH%is%is" % (g,l), CMD_GAME, command, g+l+1, game, instance))
        res = self.sock.recv(1024)
        (cmd,sub,l,str) = unpack(">2BH%is" % (len(res)-4), res) 
        try:
            print "Start: ", cmd, sub, l, str, CMD_MAP[cmd][sub]
            return CMD_MAP[cmd][sub]
        except:
            print res
            exit(0);

    def update_players(self):
        print "Send: LIST_PLAYERS"
        self.sock.send(pack(">2BH", CMD_GAME, CMD_LIST_PLAYERS, 0))

    def set_status(self, status): self.status = status
    def send_command(self, command, f=0, t=0, armies=0):
        command = command.replace(' ', '_').upper()
        if (self.command_map.has_key(command)):
            cmd = self.command_map[command]
        else:
            raise RuntimeError("Unrecognized command: %s" % command)
        print "Send:", cmd, f, t, armies
        packet = pack(">4b", cmd, f, t, armies)
        self.sock.send(packet)

    def recv_command(self):
        try:
            cmd = self.sock.recv(4)
        except:
            return (False,None)
        c = cmd_cmd(cmd)
        print "Recv:", ord(cmd[0]), ord(cmd[1]), ord(cmd[2]), ord(cmd[3])
        if (c == CMD_GAME):
            (s, msg_len) = unpack(">xBH", cmd)
            print "Reading an additional %i bytes" % msg_len
            self.sock.setblocking(1)
            msg = self.sock.recv(msg_len)
            self.sock.setblocking(0)
            if s == CMD_LIST_PLAYERS:
                print "Players => %s" % msg
                names = msg.split(':')
                names.pop(0)
                self.players = len(names) / 2
                self.status.add_player(names)
                return (True, None)
            print "???", s
            return (True, None)
        elif (c == CMD_ERROR):
            print CMD_MAP[c][ord(cmd[1])]
            return (True, None)
        elif (c == ERROR):
            print ERRORS[ord(cmd[1])]
            return (True, None)
        elif (c == ATTACK_RESULT or c == MOVE_RESULT):
            self.sock.setblocking(1)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            msg = self.sock.recv(4)
            cid = int(ord(msg[0]))
            new_owner = ord(msg[1])
            old_owner = self.countries[cid].owner
            self.countries[cid].update(msg)
            if new_owner != old_owner:
                if new_owner == self.player_id:
                    self.ui.finish_attack(cid)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == GET_ARMIES):
            self.reserve = cmd_armies(cmd)
            return (True, 'GET_ARMIES')
        elif (c == COUNTRY_STATUS):
            self.sock.setblocking(1)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == GAME_STATUS):
            lands = self.countries
            self.sock.setblocking(1)
            for i in range(42):
                msg = self.sock.recv(4)
                self.countries[ord(msg[0])].update(msg)
            self.sock.setblocking(0)
            return (True, 'UPDATE')
        elif (c == PLAYER_STATUS):
            self.player_id = cmd_from(cmd)
            self.players = cmd_to(cmd)
            self.reserve = cmd_armies(cmd)
        elif (c == READY):
            self.status.set_ready(cmd_from(cmd), True)
            return (True, 'READY')
        elif (c == NOTREADY):
            self.status.set_ready(cmd_from(cmd), False)
            return (True, 'NOTREADY')
        elif (c == PLAYER_JOIN):
            self.players = self.players + 1
            return (True, 'PLAYER_JOIN')
        elif (c == PLAYER_QUIT):
            self.players = self.players - 1
            self.status.players[cmd_from(cmd)].playing = False
            return (True, 'PLAYER_QUIT')
        elif (c == START_PLACING):
            for p in range(self.players):
                self.status.set_ready(p, False)
            self.state = "Placing Armies"
        elif (c == BEGIN):
            for p in range(self.players):
                self.status.set_ready(p, False)
            self.state = "At war"
        elif (c == DEFEAT):
            self.status.players[cmd_from(cmd)].set_result(False)
            self.status.redraw()
        elif (c == VICTORY):
            self.status.players[cmd_from(cmd)].set_result(True)
            self.state = "Game Over"
        try:
            return (True, COMMANDS[c])
        except:
            print cmd
            return (True, None)

class Country(Observable):
    def __init__(self, cmd):
        (self.id, self.owner, self.armies) = unpack(">BBBx", cmd)

    def update(self, cmd):
        (self.id, self.owner, self.armies) = unpack(">BBBx", cmd)

    def __repr__(self):
        return "[%15s: %i]" % (color(self.owner + 31, COUNTRIES[self.id]), self.armies)

def color(code, str):
    return "%c[%im%s%c[0m" % (27, code, str, 27)
