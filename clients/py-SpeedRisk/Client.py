import socket
from struct import pack, unpack

verbose = False
warnings = False

def _deserialize(data):
    t = ord(data[0])
    if t == 0: #NULL
        return (None, 1)
    if t == 1: #String
        l = ord(data[1])
        return (data[2:2+l], l+2)
    if t == 2: #Byte
        return (ord(data[1]), 2)
    if t == 3: #Array
        ind = 2
        l = ord(data[1])
        arr = []
        for i in range(l):
            (obj, used) = _deserialize(data[ind:])
            arr.append(obj)
            ind = ind + used
        return (arr, ind)
    raise BaseException("Unknown serialization type %i" % t)

class Client:
    _commands = {}

    def __init__(self, host, port, name):
        self.sock = socket.socket()
        self.sock.connect((host, port))
        self.rename(name)
        self.sock.setblocking(0)

    def poll(self):
        while self.poll_one():
            pass

    def poll_one(self):
        global warnings
        cmd = None
        try:
            cmd = self.sock.recv(4)
        except:
            return False
        (major, minor, msg_len) = unpack(">2bH", cmd)
        msg = None
        if msg_len > 0:
            self.sock.setblocking(1)
            msg = self.sock.recv(msg_len)
            self.sock.setblocking(0)
        handler = None
        try:
            handler = Client._commands[major, minor]
        except KeyError:
            print "No handler for %i %i" % (major, minor)
            return
        try:
            h = getattr(self, handler)
            h(msg)
        except AttributeError:
            if warnings:
                print "No callback for %s" % handler
        return True

    def deserialize(self, data):
        (obj, size) = _deserialize(data)
        if size < len(data):
            #with open("x.bin", "w") as f:
            #    f.write(data)
            #print obj, size, len(data)
            raise BaseException("Bad message")
        return obj

def __command(major, minor, name):
    def wrapped(self, *msg):
        print name, msg
        if len(msg) > 0:
            l = len(msg[0])
            self.sock.send(pack(">2BH%is" % l, major, minor, l, msg[0]))
        else:
            self.sock.send(pack(">2BH", major, minor, 0))
    setattr(Client, name, wrapped)

    Client._commands[major, minor] = "on_%s" % name

__command(1, 0, 'invalid_command')
__command(2, 0, 'game_full')
__command(2, 1, 'no_game')
__command(2, 2, 'in_game')
__command(4, 0, 'rename')
__command(5, 0, 'list_games')
__command(5, 1, 'list_game_instances')
__command(5, 2, 'create_game')
__command(5, 3, 'join_game')
__command(5, 4, 'list_players')
__command(5, 5, 'quit_game')
__command(5, 6, 'game_message')

# ex: set ts=4 expandtab: set sw=4
