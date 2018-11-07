#!/usr/bin/python2.7
import os
import sys
import pygame
import string
import argparse
from collections import namedtuple

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("image")
    parser.add_argument("--name", required=True)
    parser.add_argument("--hotspot-x", type=int, default=0)
    parser.add_argument("--hotspot-y", type=int, default=0)
    parser.add_argument("-o", "--output", required=True)
    opts = parser.parse_args()

    pygame.init()
    image = pygame.image.load(opts.image)
    height, width = image.get_height(), image.get_width()
    if width != 32:
        print "Can't use image, needs to be 32 pixels wide"

    image_words = []
    mask_words = []

    for row in xrange(height):
        data = 0
        mask = 0
        for column in xrange(32):
            coord = column, row
            r, g, b, a = image.get_at(coord)
            data |= (1 if r else 0) << column
            mask |= (1 if a else 0) << column
        image_words.append(hex(data))
        mask_words.append(hex(mask))

    header = os.path.join(opts.output, "{}.h".format(opts.name))
    body = os.path.join(opts.output, "{}.c".format(opts.name))
    with open(header, "w") as outf:
        outf.write("""// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "sprite.h"

extern sprite_t {name};""".format(name=opts.name))
    with open(body, "w") as outf:
        outf.write("""#include "{name}.h"
static uint32_t image_data[] = {{ {image_words} }};
static uint32_t mask_data[] = {{ {mask_words} }};
sprite_t {name} = {{
        .height={height},
        .hotspot_x={hotspot_x},
        .hotspot_y={hotspot_y},
        .image_modulo=0,
        .mask_modulo=0,
        .image=image_data,
        .mask=mask_data
        }};""".format(
            name=opts.name,
            image_words=", ".join(image_words),
            mask_words=", ".join(mask_words),
            height=height,
            hotspot_x=opts.hotspot_x,
            hotspot_y=opts.hotspot_y,
            )
        )

if __name__ == '__main__':
    main()
