#ifndef DMG_16BIT_H
#define DMG_16BIT_H

#include <SDL3/SDL.h>

Uint16 DMG_ComposeUint16(Uint8 less_significant_byte, Uint8 more_significant_byte);

void DMG_DecomposeUint16(Uint16 source, Uint8* less_significant_byte, Uint8* more_significant_byte);

#endif//DMG_16BIT_H
