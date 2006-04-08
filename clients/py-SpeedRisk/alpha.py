#! /usr/local/bin/python

import pygame
import sys

if len(sys.argv) < 2:
    print "Usage: alpha.py image"
    sys.exit()
print "Printing alpha channel for", sys.argv[1]
i = pygame.image.load(sys.argv[1])
a = pygame.surfarray.pixels_alpha(i)
s = a.tolist()
for l in s:
    for i in l:
        sys.stdout.write("%3i " % i)
    print ""
