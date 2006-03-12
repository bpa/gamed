#!/usr/local/bin/python

import socket, os
from struct import pack, unpack
from sys import stdout
from exceptions import RuntimeError
import pygame
from pygame.locals import *
import traceback
from Borders import borders
from observing import Observable

color = pygame.color.THECOLORS
PLAYER_COLORS = (color['darkolivegreen'][:-1],
                 color[     'firebrick'][:-1],
                 color[    'mediumblue'][:-1],
                 color[          'gold'][:-1],
                 color[        'purple'][:-1],
                 color[         'white'][:-1])

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
            self.countries.append(Country(pack('>4b',c,0,0,0)))

    def send_command(self, command, f=0, t=0, armies=0):
        command = command.replace(' ', '_').upper()
        if (self.command_map.has_key(command)):
            cmd = self.command_map[command]
            print command
        else:
            raise ClientError("Unrecognized command: %s" % command)
        packet = pack(">4b", cmd, f, t, armies)
        self.sock.send(packet, 4)

    def recv_command(self):
        try:
            cmd = self.sock.recv(4,socket.MSG_DONTWAIT)
        except:
            return (False,None)
        c = ord(cmd[0])
        if (c == self.command_map['ERROR']):
            print ERRORS[ord(cmd[1])]
            return (True, None)
        elif (c == 12 or c == 13):
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            return (True, 'UPDATE')
        elif (c == self.command_map['COUNTRY_STATUS']):
            msg = self.sock.recv(4)
            self.countries[ord(msg[0])].update(msg)
            return (True, 'UPDATE')
        elif (c == self.command_map['GAME_STATUS']):
            lands = self.countries
            for i in range(42):
                msg = self.sock.recv(4)
                self.countries[ord(msg[0])].update(msg)
            return (True, 'UPDATE')
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

class Player:
    def __init__(self, color, icon):
        self.color = color

class CountryDisplay(pygame.sprite.Sprite):
    def __init__(self, group, img, x, y, country):
        pygame.sprite.Sprite.__init__(self, group)
        self.rect = img.get_rect().move(x,y)
        self.image = img
        self.country = country
        self.name = COUNTRIES[country.id]
        self.owner = -1
        self.sprites = [self]
        self.selected = False
        self.token = font.render(str(self.country.armies), True, fontcolor)
        self.country.add_observer(self,['armies'])

    def handle_observation(self, c, field, old, new):
        self.token = font.render(str(new), True, fontcolor)

    def set_selected(self, selected):
        self.selected = selected
        color = PLAYER_COLORS[self.owner]
        if selected:
           color = (255 - color[0], 255 - color[1], 255 - color[2])
        for s in self.sprites:
            c = pygame.surfarray.pixels3d(s.image)
            c[::] = color
            del c

    def update(self, screen):
        if self.owner != self.country.owner:
            self.owner = self.country.owner
            color = PLAYER_COLORS[self.owner]
            for s in self.sprites:
                c = pygame.surfarray.pixels3d(s.image)
                c[::] = color
                del c
        screen.blit(self.image, self.rect)
        screen.blit(self.token, self.rect.center)

    def add_image(self, img, x, y):
        sprite = pygame.sprite.Sprite(self.groups())
        sprite.image = img
        sprite.rect = img.get_rect().move(x,y)
        sprite.country = self.country
        sprite.name = self.name
        sprite.set_selected = self.set_selected
        self.sprites.append(sprite)
        self.owner = -1

class CountryGroup(pygame.sprite.RenderPlain):
    def collidepoint(self, point):
        for s in self.spritedict.keys():
            if s.rect.collidepoint(point):
                loc = (point[0] - s.rect.left,
                       point[1] - s.rect.top)
                pixel = s.image.get_at(loc)
                if pixel[3] > 0:
                    return s
        return None

class SpeedRiskUI:
    def __init__(self):
        #pygame.key.set_repeat(500,15)
        #pygame.display.list_modes(0,FULLSCREEN|HWACCEL)
        ss = pygame.Rect(0,0,650,375)
        self.screen_size = ss
        self.screen = pygame.display.set_mode(ss.size,HWACCEL)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None,20)
        self.fg = pygame.color.Color('white')
        self.bg = pygame.color.Color('black')
        self.client = Client()
        self.load_images()
        self.selected_country = None
        self.choosing_count = False
        self.picker = pygame.sprite.RenderPlain()
        self.last_click = 0
        self.action = None
        self.action_from = 0
        self.action_to = 0

    def run(self):
        self.running = True
        while self.running:
            self.draw_screen()
            self.handle_user_events()
            try:
                reading = True
                while reading:
                    (reading, action) = self.client.recv_command()
                    if reading: print action
            except Exception, e:
                traceback.print_exc()
            self.clock.tick(40);

    def load_images(self):
        self.scale = pygame.sprite.Sprite()
        self.scale.image = pygame.image.load('scale.png')
        self.scale.rect = self.scale.image.get_rect()
        self.overlays = CountryGroup()
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
        self.overlays.update(self.screen)
        self.picker.draw(self.screen)
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
            elif event.type == MOUSEBUTTONDOWN:
                if event.button == 1:
                    if self.choosing_count:
                        self.choosing_count = False
                        self.picker.empty()
                        if self.scale.rect.collidepoint(event.pos):
                            self.complete_action()
                            return
                    c = self.overlays.collidepoint(event.pos)
                    if c == None:
                        if self.selected_country != None:
                            self.selected_country.set_selected(False)
                            self.selected_country = None
                    else:
                        now = pygame.time.get_ticks()
                        if self.selected_country == c:
                            if now < self.last_click + 250:
                                self.init_place(event.pos, c)
                        else:
                            if self.selected_country != None:
                                self.selected_country.set_selected(False)
                                if borders(self.selected_country, c):
                                    self.init_move_or_attack(event.pos,c)
                            self.selected_country = c
                            c.set_selected(True)
                        self.last_click = now

    def complete_action(self):
        armies = 1
        self.client.send_command(self.action, self.action_from, self.action_to, armies)

    def init_place(self, pos, to_country):
        self.picker.add(self.scale)
        self.choosing_count = True
        self.scale.rect.topleft = pos
        self.scale.rect.clamp_ip(self.screen_size)
        self.action = 'PLACE';
        self.action_to = to_country.country.id

    def init_move_or_attack(self, pos, to_country):
        from_country = self.selected_country
        f_id = from_country.country.id
        t_id =   to_country.country.id
        if from_country.owner == to_country.owner:
            self.picker.add(self.scale)
            self.choosing_count = True
            self.scale.rect.topleft = pos
            self.scale.rect.clamp_ip(self.screen_size)
            self.action = 'MOVE';
            self.action_from = f_id
            self.action_to   = t_id
        else:
            armies = min(from_country.country.armies - 1, 3)
            self.client.send_command('ATTACK', f_id, t_id, armies)

if __name__ == "__main__":
    pygame.init()
    font = pygame.font.Font(None, 20)
    fontcolor = pygame.color.Color('black')
    ui = SpeedRiskUI()
    ui.run()
