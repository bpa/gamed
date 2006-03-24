#!/usr/local/bin/python

import os
import pygame
from pygame.locals import *

black = pygame.color.Color('black')

class Country(pygame.sprite.Sprite):
    def __init__(self,group=None):
        pygame.sprite.Sprite.__init__(self, group)

    def update(self, screen):
        screen.blit(self.image, self.rect)
        screen.blit(num, self.label)

    def set_selected(self, selected):
        if selected:
           color = pygame.color.THECOLORS['red'][:-1]
        else:
           color = pygame.color.THECOLORS['darkolivegreen'][:-1]
        c = pygame.surfarray.pixels3d(self.image)
        c[::] = color
        del c

class SpeedRiskClient:
    def __init__(self):
        pygame.key.set_repeat(500,15)
        #pygame.display.list_modes(0,FULLSCREEN|HWACCEL)
        self.screen = pygame.display.set_mode((720,400),FULLSCREEN|HWACCEL)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None,20)
        self.fg = pygame.color.Color('white')
        self.bg = pygame.color.Color('black')
        self.blue = pygame.color.Color('blue')
        self.load_images()
        self.login()

    def run(self):
        self.running = True
        while self.running:
            self.draw_screen()
            self.handle_user_events()
            self.handle_network_events()
            self.clock.tick(40);
        self.write_manifest()

    def load_images(self):
        self.overlays = pygame.sprite.RenderPlain()
        self.overlay_info = []
        #print self.background.get_size()
        os.chdir('overlays')
        self.background = pygame.image.load('world_map_relief.jpg')
        countries = open('manifest')
        for c in countries.readlines():
            (id, name, x, y, lx, ly) = c.split(':')
            sprite = Country(self.overlays)
            sprite.id = int(id)
            sprite.name = name
            sprite.image = pygame.image.load(name)
            sprite.rect = sprite.image.get_rect().move(int(x),int(y))
            sprite.label = pygame.Rect(int(lx), int(ly), 0, 0)
            sprite.set_selected(False)
            self.overlay_info.append(sprite)
        self.active_country = 0
        self.overlay_info[0].set_selected(True)

    def write_manifest(self):
        file = open('manifest', 'w')
        for s in self.overlay_info:
            file.write("%i:%s:%i:%i:%i:%i\n" % (s.id, s.name, s.rect.left, s.rect.top, s.label.left,s.label.top))

    def login(self):
        pass

    def draw_screen(self):
        self.screen.fill(self.bg)
        self.screen.blit(self.background,(0,0))
        self.overlays.update(self.screen)
        active = self.font.render(self.overlay_info[self.active_country].name, 0, self.fg)
        self.screen.blit(active,(10,380))
        pygame.display.flip()

    def handle_user_events(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                self.running = False
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    self.running = False
                elif event.key == K_UP:
                    self.overlay_info[self.active_country].label.move_ip( 0,-1)
                elif event.key == K_DOWN:
                    self.overlay_info[self.active_country].label.move_ip( 0, 1)
                elif event.key == K_LEFT:
                    self.overlay_info[self.active_country].label.move_ip(-1, 0)
                elif event.key == K_RIGHT:
                    self.overlay_info[self.active_country].label.move_ip( 1, 0)
                elif event.key == ord('k'):
                    self.overlay_info[self.active_country].set_selected(False)
                    self.active_country = self.active_country + 1
                    if self.active_country >= len(self.overlay_info):
                        self.active_country = 0
                    self.overlay_info[self.active_country].set_selected(True)
                elif event.key == ord('j'):
                    self.overlay_info[self.active_country].set_selected(False)
                    self.active_country = self.active_country - 1
                    if self.active_country < 0:
                        self.active_country = len(self.overlay_info) - 1
                    self.overlay_info[self.active_country].set_selected(True)

    def handle_network_events(self):
        pass

if __name__ == "__main__":
    pygame.init()
    font = pygame.font.Font(None, 20)
    num = font.render("255", True, black)
    client = SpeedRiskClient()
    client.run()
