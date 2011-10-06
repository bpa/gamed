#!/usr/local/bin/python

import pygame

color = pygame.color.THECOLORS
for c in color:
    rgb = pygame.color.Color(c)
    weight = rgb[0] * 30 + rgb[1] * 59 + rgb[2] * 11
    print weight, c
#1     0    0 black
#2  3643 3457 midnightblue
#3  7286 7720 firebrick
#4 10928 11073 ----Don't use----
#5 14571 14795 peru
#6 18214 18588 lawngreen
#7 21857 21187 aquamarine
#8 25500 white 25500
