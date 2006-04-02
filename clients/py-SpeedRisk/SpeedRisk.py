#!/usr/local/bin/python

import os
import pygame
import traceback
from pygame.locals import *
from Borders import borders
from math import ceil
from Client import *

dirty = True
color = pygame.color.THECOLORS
PLAYER_COLORS = (color['darkolivegreen'][:-1],
                 color[     'firebrick'][:-1],
                 color[    'mediumblue'][:-1],
                 color[          'gold'][:-1],
                 color[        'purple'][:-1],
                 color[         'white'][:-1])

class CountryDisplay(pygame.sprite.Sprite):
    def __init__(self, group, img, x, y, lx, ly, country):
        pygame.sprite.Sprite.__init__(self, group)
        self.sprites = [self]
        self.setup(img, x, y, lx, ly, country, False)
        self.country.add_observer(self,['armies','owner'])
        self.render_token()

    def add_image(self, img, x, y, lx, ly):
        sprite = pygame.sprite.Sprite(self.groups())
        self.sprites.append(sprite)
        sprite.set_selected = self.set_selected
        self.setup(img, x, y, lx, ly, self.country, self.selected, sprite)

    def setup(self, img, x, y, lx, ly, country, selected, sprite=None):
        '''I apologize in advance for this method.  Apparently, there is 
             type checking for object methods, and you can't just pass
             something in that supports all the operations of the base object'''
        if sprite != None: self = sprite
        self.country = country
        self.name = COUNTRIES[country.id]
        self.image = img
        self.rect = img.get_rect().move(x,y)
        self.set_selected(selected)
        self.token_pos = (lx, ly)

    def render_token(self):
        self.token = font.render(str(self.country.armies), True, black)

    def handle_observation(self, c, field, old, new):
        global dirty
        if field == 'armies':
            self.render_token()
            dirty = True
        else: # field == 'owner'
            self.set_selected(self.selected)

    def set_selected(self, selected):
        global dirty
        dirty = True
        self.selected = selected
        color = PLAYER_COLORS[self.country.owner]
        if selected:
            reverse = (255 - color[0], 255 - color[1], 255 - color[2])
            for s in self.sprites:
                c = pygame.surfarray.pixels3d(s.image)
                #c[1::2,1::2] = (0,0,0)
                c[::] = (0,0,0)
                c[ ::2, ::2] = color
                c[1::4,1::4] = color
                del c
        else:
            for s in self.sprites:
                c = pygame.surfarray.pixels3d(s.image)
                c[::] = color
                del c

    def update(self, screen):
        for s in self.sprites:
            screen.blit(s.image, s.rect)
            screen.blit(self.token, s.token_pos)

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

class Status:
    def __init__(self, client):
        client.add_observer(self, ['players','state','reserve'])
        client.set_status(self)
        self.client = client
        self.handle_observation(client, None, None, None)
        self.players = []
        self.update_players()

    def handle_observation(self, c, field, old, new):
        global dirty
        self.image = big.render("%s\n %i armies in reserve " % (c.state, c.reserve), True, black, PLAYER_COLORS[c.player_id])
        self.rect = self.image.get_rect().move(250,300)
        dirty = True

    def draw(self, screen):
        screen.blit(self.image, self.rect)
        for s in self.players:
            screen.blit(s.image, s.rect)

    def update_players(self):
        self.client.sock.send("/players")

    def add_player(self, cmd):
        global dirty
        if len(self.players) < self.client.players:
            dirty = True
            sprite = pygame.sprite.Sprite()
            self.players.append(sprite)
            id = len(self.players)
            sprite.id = id
            sprite.image = font.render("Player %i" % id, True, PLAYER_COLORS[id-1])
            sprite.rect = self.image.get_rect()
            scr_rect = pygame.display.get_surface().get_rect()
            sprite.rect.topleft = (20, scr_rect.bottom - (20 * (id + 1)))

class Picker:
    def __init__(self, screen_size):
        self.screen_size = screen_size
        self.active = False
        self.group = pygame.sprite.RenderPlain()
        self.scale = pygame.sprite.Sprite()
        self.scale.image = pygame.image.load('scale.png')
        self.scale.rect = self.scale.image.get_rect()
        self.armies = 0
        self.range_upper = pygame.sprite.Sprite(self.group)
        self.range_lower = pygame.sprite.Sprite(self.group)
        self.render_range()

    def render_range(self):
        self.range_upper.image = font.render(
            str(self.armies), True, black)
        self.range_upper.rect = self.range_upper.image.get_rect()
        self.range_upper.rect.midtop = self.scale.rect.midtop

        self.range_lower.image = font.render(
            '1', True, black)
        self.range_lower.rect = self.range_lower.image.get_rect()
        self.range_lower.rect.midbottom = self.scale.rect.midbottom

    def draw(self, screen):
        if self.active:
            screen.blit(self.scale.image, self.scale.rect)
            self.group.draw(screen)
            
    def activate(self, pos, armies):
        global dirty
        dirty = True
        self.active = True
        self.scale.rect.topleft = pos
        self.scale.rect.clamp_ip(self.screen_size)
        self.armies = armies
        self.render_range()

    def deactivate(self):
        global dirty
        if self.active:
            self.active = False
            dirty = True

    def armies_chosen(self, point):
        if self.active:
            if self.scale.rect.collidepoint(point):
                height = float(self.scale.rect.bottom - point[1])
                return int(ceil(height / self.scale.rect.height * self.armies))
        else:
            return 0

class SpeedRiskUI:
    def __init__(self):
        #pygame.key.set_repeat(500,15)
        #pygame.display.list_modes(0,FULLSCREEN|HWACCEL)
        ss = pygame.Rect(0,0,650,375)
        self.screen_size = ss
        self.screen = pygame.display.set_mode(ss.size,HWACCEL)
        self.clock = pygame.time.Clock()
        self.fg = pygame.color.Color('white')
        self.bg = pygame.color.Color('black')
        self.client = Client()
        self.status = Status(self.client)
        self.client.add_observer(self, ['players'])
        self.load_images()
        self.picker = Picker(ss)
        self.selected_country = None
        self.choosing_count = False
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
                    if reading:
                        if action != None:
                            print action
                        if action == 'UPDATE':
                            if self.selected_country != None:
                                if self.selected_country.country.owner \
                                        != self.client.player_id:
                                    self.selected_country.set_selected(False)
                                    self.selected_country = None
            except Exception, e:
                traceback.print_exc()
            self.clock.tick(40);

    def load_images(self):
        self.overlays = CountryGroup()
        self.overlay_info = []
        for i in range(42): self.overlay_info.append(None)
        os.chdir('images')
        self.background = pygame.image.load('world_map_relief.jpg')
        countries = open('manifest')
        for c in countries.readlines():
            (id, name, x, y, lx, ly) = c.split(':')
            id = int(id)
            image = pygame.image.load(name)
            if self.overlay_info[id] != None:
                self.overlay_info[id].add_image(image, \
                    int(x), int(y), int(lx), int(ly))
            else:
                obj = self.client.countries[id]
                self.overlay_info[id] = \
                    CountryDisplay(self.overlays, image, \
                        int(x), int(y), int(lx), int(ly), obj)

    def draw_screen(self):
        global dirty
        #self.screen.fill(self.bg)
        if dirty:
            self.screen.blit(self.background,(0,0))
            self.overlays.update(self.screen)
            players.draw(self.screen)
            self.status.draw(self.screen)
            self.picker.draw(self.screen)
            pygame.display.flip()
            dirty = False

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
                    chosen = self.picker.armies_chosen(event.pos)
                    if chosen > 0:
                        self.client.send_command(self.action, self.action_from, 
                            self.action_to, chosen)
                        self.picker.deactivate()
                        return
                    c = self.overlays.collidepoint(event.pos)
                    if c == None:
                        if self.selected_country != None:
                            self.selected_country.set_selected(False)
                            self.selected_country = None
                            self.picker.deactivate()
                    else:
                        now = pygame.time.get_ticks()
                        if self.selected_country == c:
                            if now < self.last_click + 250:
                                self.init_place(event.pos, c)
                            else:
                                self.picker.deactivate()
                        else:
                            if self.selected_country != None:
                                if borders(self.selected_country, c):
                                    self.init_move_or_attack(event.pos,c)
                                else:
                                    self.picker.deactivate()
                            if c.country.owner == self.client.player_id:
                                if self.selected_country != None:
                                    self.selected_country.set_selected(False)
                                self.selected_country = c
                                c.set_selected(True)
                        self.last_click = now

    def init_place(self, pos, to_country):
        if self.client.reserve > 0:
            self.picker.activate(pos, self.client.reserve)
            self.choosing_count = True
            self.action = 'PLACE';
            self.action_to = to_country.country.id
        else:
            self.picker.deactivate()

    def init_move_or_attack(self, pos, to_country):
        from_country = self.selected_country
        f_id = from_country.country.id
        t_id =   to_country.country.id
        armies_available = from_country.country.armies - 1
        if armies_available > 0:
            if from_country.country.owner == to_country.country.owner:
                self.picker.activate(pos, armies_available)
                self.action = 'MOVE';
                self.action_from = f_id
                self.action_to   = t_id
            else:
                armies = min(armies_available, 3)
                self.client.send_command('ATTACK', f_id, t_id, armies)
                self.picker.deactivate()
        else:
            self.picker.deactivate()

    def handle_observation(self, c, field, old, new):
        #if field == 'players':
            self.status.update_players()

if __name__ == "__main__":
    pygame.init()
    font = pygame.font.Font(None, 20)
    big = pygame.font.Font(None, 40)
    black = pygame.color.Color('black')
    players = pygame.sprite.RenderPlain()
    ui = SpeedRiskUI()
    ui.run()
