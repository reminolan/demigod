/*
 * Copyright (c) 2026 Remi Nolan
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef DMG_INTEGERS_H
#define DMG_INTEGERS_H

#include <SDL3/SDL.h>

Uint16 DMG_ComposeUint16(Uint8 less_significant_byte, Uint8 more_significant_byte);

void DMG_DecomposeUint16(Uint16 source, Uint8* less_significant_byte, Uint8* more_significant_byte);

#endif//DMG_INTEGERS_H
