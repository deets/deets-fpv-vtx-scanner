import os
import sys
import pygame
import string
from collections import namedtuple

Glyph = namedtuple("Glyph", "image mask hotspot_x")
pygame.init()

font = pygame.font.Font(sys.argv[1], 8)
name = os.path.splitext(os.path.basename(sys.argv[1]))[0]

all_chars = "".join(chr(i) if chr(i) in string.printable else " " for i in xrange(128))

text = font.render(all_chars, True, (255, 255, 255), (0, 0, 0))
height, width = text.get_height(), text.get_width()
if width % 128:
    print "Can't use font because it's proportional!"

charwidth = width / 128
# for now, we only support really 8 pixel wide fonts. Might change in the futurea
assert charwidth == 8

glyphs_per_word = 32 / charwidth
word_number = width / 32
image_words = []
masks = []
glyphs = []

for i in xrange(glyphs_per_word):
    mask = bin(((1 << charwidth) - 1) << charwidth * i)
    masks.append(mask)


for row in xrange(height):
    for word in xrange(word_number):
        data = 0
        for column in xrange(32):
            coord = (word * 32 + column, row)
            pixel = text.get_at(coord)[0]
            data |= (1 if pixel else 0) << column
        image_words.append(hex(data))
        if row == 0:
            for glyph in xrange(glyphs_per_word):
                glyphs.append(Glyph(
                    image=word,
                    mask=glyph,
                    hotspot_x=charwidth*glyph
                    ))

glyph_string = ",\n".join(
"""{{
      .image=font_data + {},
      .mask=font_masks + {},
      .image_modulo={},
      .mask_modulo=-1,
      .height={},
      .hotspot_x={},
      .hotspot_y=0
    }}""".format(glyph.image, glyph.mask, word_number - 1, height, glyph.hotspot_x)
    for glyph in glyphs)

print '#include "p2font.h"'
print "uint32_t font_data[] = {" + ", ".join(image_words) + "};"
print "uint32_t font_masks[] = {" + ", ".join(masks) + "};"
print "font_t p2font = { .glyphs={" + glyph_string + "}};";
