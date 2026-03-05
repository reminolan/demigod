#include "demigod.h"

Uint16 DMG_ComposeUint16(Uint8 less_significant_byte, Uint8 more_significant_byte) {
   #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
   return (less_significant_byte << 8) | more_significant_byte;
   #elif SDL_ENDIANNESS == SDL_BIG_ENDIAN
   return (more_significant_byte << 8) | less_significant_byte;
   #else
   return 0;
   #endif
}

void DMG_DecomposeUint16(Uint16 source, Uint8* out_less_significant_byte, Uint8* out_more_significant_byte) {
   Uint8 less_significant = 0;
   Uint8 more_significant = 0;

   #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
   less_significant = (Uint8)(source >> 8);
   more_significant = (Uint8)(source);
   #elif SDL_ENDIANNESS == SDL_BIG_ENDIAN
   less_significant = (Uint8)(source);
   more_significant = (Uint8)(source >> 8);
   #endif

   if (out_less_significant_byte)
      *out_less_significant_byte = less_significant;
   if (out_more_significant_byte)
      *out_more_significant_byte = more_significant;
}

