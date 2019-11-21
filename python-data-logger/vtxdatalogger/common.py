# -*- coding: utf-8 -*-
# Copyright: 2019, Diez B. Roggisch, Berlin . All rights reserved.
from enum import unique, IntEnum

TOTAL = 4

@unique
class Mode(IntEnum):
    SPLASH_SCREEN, SCANNER, LAPTIMER, SETTINGS = range(TOTAL)
    DISCONNECTED = TOTAL  # This is custom and signifies BT troubles
