#!/usr/local/bin/python

import os
import pygame
from pygame.locals import *

class SpeedRiskClient:
    def __init__(self):
        pygame.init()
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
        self.background = pygame.image.load('world_map_relief.jpg')
        self.overlays = pygame.sprite.RenderPlain()
        self.overlay_info = []
        #print self.background.get_size()
        os.chdir('overlays')
        countries = open('manifest')
        for c in countries.readlines():
            (name, x, y) = c.split(':')
            sprite = pygame.sprite.Sprite(self.overlays)
            sprite.name = name
            sprite.image = pygame.image.load(name)
            c = pygame.surfarray.pixels3d(sprite.image)
            c[::] = (255,0,0)
            del c
            sprite.rect = sprite.image.get_rect().move(int(x),int(y))
            self.overlay_info.append(sprite)
        self.active_country = 0

    def write_manifest(self):
        file = open('manifest', 'w')
        for s in self.overlay_info:
            file.write("%s:%i:%i\n" % (s.name, s.rect.left, s.rect.top))

    def login(self):
        pass

    def draw_screen(self):
        self.screen.fill(self.bg)
        self.screen.blit(self.background,(0,0))
        self.overlays.draw(self.screen)
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
                    self.overlay_info[self.active_country].rect.move_ip( 0,-1)
                elif event.key == K_DOWN:
                    self.overlay_info[self.active_country].rect.move_ip( 0, 1)
                elif event.key == K_LEFT:
                    self.overlay_info[self.active_country].rect.move_ip(-1, 0)
                elif event.key == K_RIGHT:
                    self.overlay_info[self.active_country].rect.move_ip( 1, 0)
                elif event.key == ord('k'):
                    self.active_country = self.active_country + 1
                    if self.active_country >= len(self.overlay_info):
                        self.active_country = 0
                elif event.key == ord('j'):
                    self.active_country = self.active_country - 1
                    if self.active_country < 0:
                        self.active_country = len(self.overlay_info) - 1

    def handle_network_events(self):
        pass

if __name__ == "__main__":
    client = SpeedRiskClient()
    client.run()
