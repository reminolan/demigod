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

#ifndef DMG_CPU_H
#define DMG_CPU_H

#include <SDL3/SDL.h>

#include "dmg_cpu_opcodes.h"

typedef enum DMG_CPUFlags: Uint8 {
   DMG_CPU_FLAG_ZERO               = 0b01000000,
   DMG_CPU_FLAG_ZERO_SHIFT         = 6,

   DMG_CPU_FLAG_SUBTRACTION        = 0b00100000,
   DMG_CPU_FLAG_SUBTRACTION_SHIFT  = 5,

   DMG_CPU_FLAG_HALF_CARRY         = 0b00010000,
   DMG_CPU_FLAG_HALF_CARRY_SHIFT   = 4,

   DMG_CPU_FLAG_CARRY              = 0b00001000,
   DMG_CPU_FLAG_CARRY_SHIFT        = 3,
} DMG_CPUFlags;

typedef struct DMG_RegisterFile {
   Uint8 instruction;
   Uint8 interrupt_enable;

   union {
      Uint16 af;

      struct {
         #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
         Uint8 f, a;
         #else
         Uint8 a, f;
         #endif
      };

      struct {
         #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
         DMG_CPUFlags flags;
         Uint8 accumulator;
         #else
         Uint8 accumulator;
         DMG_CPUFlags flags;
         #endif
      };
   };

   union {
      Uint16 bc;

      struct {
         #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
         Uint8 c, b;
         #else
         Uint8 b, c;
         #endif
      };
   };

   union {
      Uint16 de;

      struct {
         #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
         Uint8 e, d;
         #else
         Uint8 d, e;
         #endif
      };
   };

   union {
      Uint16 hl;

      struct {
         #if SDL_ENDIANNESS == SDL_LIL_ENDIAN
         Uint8 l, h;
         #else
         Uint8 h, l;
         #endif
      };
   };

   union {
      Uint16 program_counter;
      Uint16 pc;
   };
   union {
      Uint16 stack_pointer;
      Uint16 sp;
   };
} DMG_RegisterFile;

typedef struct DMG_CPU {
   DMG_RegisterFile registers;
   Uint8 next_cycle_interrupt_change;
} DMG_CPU;



void DMG_FetchInstruction(DMG_CPU* cpu);

void DMG_ExecuteInstruction(DMG_CPU* cpu);

#endif//DMG_CPU_H
