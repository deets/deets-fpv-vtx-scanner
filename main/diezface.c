#include "diezface.h"
static uint32_t image_data[] = { 0x400, 0xfc0a00, 0x3f00080, 0x3e00000, 0x7800000, 0x1e000010, 0x5e3fe000, 0x38fffe08, 0x31ffff00, 0x33ffff84, 0x67ffffc0, 0x4fffffc2, 0xce3ff800, 0xcfbfe001, 0x4007c000, 0x8ff80000, 0xe4c0480, 0x8e0d0480, 0x8f8f8080, 0x8fbb9cf0, 0x8ffbdff0, 0x3ff7e000, 0x4707e00d, 0xc03ff3fd, 0xdfffdbfc, 0x5ffb8ffc, 0x5fff07fd, 0x3ff803fc, 0x3fe001fc, 0x5f80106c, 0x1e1ffc1c, 0xe3c001c, 0xf8fe040, 0x1ffff880, 0x1fffe300, 0xffffc00, 0x7fffc00, 0x3ffff00, 0x3ffff00, 0x1fffc00, 0x1fff800, 0xfff880, 0x3ff000, 0x1fe000, 0x100600 };
static uint32_t mask_data[] = { 0x7ffe00, 0x1ffff00, 0xbffffc0, 0xfffffe0, 0x1ffffff0, 0x1ffffff0, 0x5ffffff8, 0x3ffffffa, 0x3ffffffc, 0x7ffffffe, 0x7ffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff, 0x7fffffff, 0x3ffffffc, 0x5ffffffc, 0x1ffffffc, 0x1ffffffc, 0x1ffffffc, 0x1ffffff8, 0x1ffffff8, 0xffffff8, 0x7fffff8, 0x7fffff0, 0xbfffff0, 0x3ffffc0, 0x1ffffc0, 0xffff80, 0x7fff00, 0x7ffe80, 0x3ffe00 };
sprite_t diezface = {
        .height=45,
        .hotspot_x=0,
        .hotspot_y=0,
        .image_modulo=0,
        .mask_modulo=0,
        .image=image_data,
        .mask=mask_data
        };