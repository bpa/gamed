#!/usr/local/bin/python

import pygame
from ui.SpeedRisk import SpeedRiskUI
from ConfigParser import RawConfigParser

if __name__ == "__main__":
    config = RawConfigParser()
    cnf_file = open("config.ini")
    config.readfp(cnf_file)
    cnf_file.close()
    host = config.get('client','host')
    port = config.getint('client','port')
    name = config.get('player','name')
    pygame.display.init()
    pygame.font.init()
    ui = SpeedRiskUI(host, port, name)
    ui.create_game("SpeedRisk:test")
    ui.join_game("SpeedRisk:test")
    ui.run()

# set sw=4 ts=4 expandtab:
