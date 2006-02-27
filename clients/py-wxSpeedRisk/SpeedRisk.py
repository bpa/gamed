#!/usr/local/bin/python

import socket, os
from struct import pack, unpack
from sys import stdout
from exceptions import RuntimeError
import pygame
from pygame.locals import *
import traceback

color = pygame.color.THECOLORS
PLAYER_COLORS = (color[     'firebrick'][:-1],
                 color['darkolivegreen'][:-1],
                 color[    'mediumblue'][:-1],
                 color[          'gold'][:-1],
                 color[        'purple'][:-1],
                 color[         'white'][:-1])

class GameError(RuntimeError):
    def __init__(self, cmd):
        self.errno = unpack(">xbxx", cmd)[0]
        self.message = ERRORS[int(self.errno)]

    def __str__(self):
        return self.message
    
class ClientError(RuntimeError):
    def __str__(self):
        return self.args.__str__()
    
class Client:
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
            self.countries.append(Country(pack('>4b',0,0,0,0)))

    def send_command(self, command, f=0, t=0, armies=0):
        command = command.replace(' ', '_').upper()
        if (self.command_map.has_key(command)):
            command = self.command_map[command]
        else:
            raise ClientError("Unrecognized command: %s" % command)
        cmd = pack(">4b", command, f, t, armies)
        self.sock.send(cmd, 4)

    def recv_command(self):
        try:
            cmd = self.sock.recv(4,socket.MSG_DONTWAIT)
        except:
            return False
        c = ord(cmd[0])
        if (c == self.command_map['ERROR']):
            raise GameError(cmd)
        elif (c == 12 or c == 13):
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            return True
        elif (c == self.command_map['COUNTRY_STATUS']):
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
        elif (c == self.command_map['GAME_STATUS']):
            lands = self.countries
            for i in range(42):
                msg = self.sock.recv(4)
                self.countries[ord(msg[0])].update(msg)
        else:
            raise ClientError(cmd)

class Country:
    def __init__(self, cmd):
        (self.id, self.owner, self.armies) = unpack(">BBBx", cmd)

    def update(self, cmd):
        (self.owner, self.armies) = unpack(">xBBx", cmd)

    def __repr__(self):
        return "[%15s: %i]" % (color(self.owner + 31, COUNTRIES[self.id]), self.armies)

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

COLORS = {'black': 30, 'red': 31, 'green': 32, 'yellow': 33, 'blue': 34,
               'magenta': 35, 'cyan': 36, 'white': 37 }

def color(code, str):
    #code = 0
    #if COLORS.has_key(c): code = COLORS[c]
    return "%c[%im%s%c[0m" % (27, code, str, 27)

class Player:
    def __init__(self, color, icon):
        self.color = color

class CountryDisplay(pygame.sprite.Sprite):
    def __init__(self, group, img, x, y, country):
        pygame.sprite.Sprite.__init__(self, group)
        self.rect = img.get_rect().move(x,y)
        self.image = img
        self.country = country
        self.owner = -1
        self.sprites = None
        self.update()

    def update(self):
        if self.owner != self.country.owner:
            self.owner = self.country.owner
            c = pygame.surfarray.pixels3d(self.image)
            c[::] = PLAYER_COLORS[self.owner]
            del c

    def add_image(self, img, x, y):
        self.update = self.update_multiple
        sprite = pygame.sprite.Sprite(self.groups())
        sprite.image = img
        sprite.rect = img.get_rect().move(x,y)
        if self.sprites == None:
            self.sprites = [self, sprite]
        else:
            self.sprites.append(sprite)

    def update_multiple(self):
        if self.owner != self.country.owner:
            self.owner = self.country.owner
            color = PLAYER_COLORS[self.owner]
            for s in self.sprites:
                c = pygame.surfarray.pixels3d(s)
                c[::] = color
                del c

class SpeedRiskUI:
    def __init__(self):
        pygame.init()
        #pygame.key.set_repeat(500,15)
        #pygame.display.list_modes(0,FULLSCREEN|HWACCEL)
        self.screen = pygame.display.set_mode((650,375),HWACCEL)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None,20)
        self.fg = pygame.color.Color('white')
        self.bg = pygame.color.Color('black')
        self.client = Client()
        self.load_images()

    def run(self):
        self.running = True
        while self.running:
            self.draw_screen()
            self.handle_user_events()
            try:
                while (self.client.recv_command()): pass
            except Exception, e:
                traceback.print_exc()
            self.clock.tick(40);

    def load_images(self):
        self.overlays = pygame.sprite.RenderPlain()
        self.overlay_info = []
        for i in range(42): self.overlay_info.append(None)
        os.chdir('overlays')
        self.background = pygame.image.load('world_map_relief.jpg')
        countries = open('manifest')
        for c in countries.readlines():
            (id, name, x, y) = c.split(':')
            id = int(id)
            image = pygame.image.load(name)
            if self.overlay_info[id] != None:
                self.overlay_info[id].add_image(image, int(x), int(y))
            else:
                obj = self.client.countries[id]
                self.overlay_info[id] = \
                    CountryDisplay(self.overlays, image, int(x), int(y), obj)

    def draw_screen(self):
        self.screen.fill(self.bg)
        self.screen.blit(self.background,(0,0))
        self.overlays.draw(self.screen)
        pygame.display.flip()

    def handle_user_events(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                self.running = False
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE or \
                   event.key == ord('q'):
                    self.running = False
                elif event.key == ord('r'):
                    self.client.send_command('READY')

if __name__ == "__main__":
    ui = SpeedRiskUI()
    ui.run()
