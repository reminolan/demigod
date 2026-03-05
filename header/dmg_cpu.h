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

typedef enum DMG_OpCode: Uint8 {
   //Control Flow
   DMG_OP_JR_e         = 0x18,
   DMG_OP_JR_Z_e       = 0x28,
   DMG_OP_JR_C_e       = 0x38,
   DMG_OP_JR_NZ_e      = 0x20,
   DMG_OP_JR_NC_e      = 0x30,

   DMG_OP_JP_HL        = 0xE9,
   DMG_OP_JP_nn        = 0xC3,
   DMG_OP_JP_Z_nn      = 0xCA,
   DMG_OP_JP_C_nn      = 0xDA,
   DMG_OP_JP_NZ_nn     = 0xC2,
   DMG_OP_JP_NC_nn     = 0xD2,

   DMG_OP_CALL_nn      = 0xCD,
   DMG_OP_CALL_Z_nn    = 0xCC,
   DMG_OP_CALL_C_nn    = 0xDC,
   DMG_OP_CALL_NZ_nn   = 0xC4,
   DMG_OP_CALL_NC_nn   = 0xD4,

   DMG_OP_RET          = 0xC9,
   DMG_OP_RET_Z        = 0xC8,
   DMG_OP_RET_C        = 0xD8,
   DMG_OP_RET_NZ       = 0xC0,
   DMG_OP_RET_NC       = 0xD0,
   DMG_OP_RETI         = 0xD9,

   DMG_OP_RST_00       = 0xC7,
   DMG_OP_RST_10       = 0xD7,
   DMG_OP_RST_20       = 0xE7,
   DMG_OP_RST_30       = 0xF7,

   DMG_OP_RST_08       = 0xCF,
   DMG_OP_RST_18       = 0xDF,
   DMG_OP_RST_28       = 0xEF,
   DMG_OP_RST_38       = 0xFF,

   //8-bit Loads
   DMG_OP_LD_BC_A      = 0x02,
   DMG_OP_LD_DE_A      = 0x12,
   DMG_OP_LD_HLi_A     = 0x22,
   DMG_OP_LD_HLd_A     = 0x32,

   DMG_OP_LD_B_n       = 0x06,
   DMG_OP_LD_D_n       = 0x16,
   DMG_OP_LD_H_n       = 0x26,
   DMG_OP_LD_HL_n      = 0x36,

   DMG_OP_LD_C_n       = 0x0E,
   DMG_OP_LD_E_n       = 0x1E,
   DMG_OP_LD_L_n       = 0x2E,
   DMG_OP_LD_A_n       = 0x3E,

   DMG_OP_LD_A_BC      = 0x0A,
   DMG_OP_LD_A_DE      = 0x1A,
   DMG_OP_LD_A_HLi     = 0x2A,
   DMG_OP_LD_A_HLd     = 0x3A,

   DMG_OP_LD_B_B       = 0x40,
   DMG_OP_LD_B_C       = 0x41,
   DMG_OP_LD_B_D       = 0x42,
   DMG_OP_LD_B_E       = 0x43,
   DMG_OP_LD_B_H       = 0x44,
   DMG_OP_LD_B_L       = 0x45,
   DMG_OP_LD_B_HL      = 0x46,
   DMG_OP_LD_B_A       = 0x47,

   DMG_OP_LD_C_B       = 0x48,
   DMG_OP_LD_C_C       = 0x49,
   DMG_OP_LD_C_D       = 0x4A,
   DMG_OP_LD_C_E       = 0x4B,
   DMG_OP_LD_C_H       = 0x4C,
   DMG_OP_LD_C_L       = 0x4D,
   DMG_OP_LD_C_HL      = 0x4E,
   DMG_OP_LD_C_A       = 0x4F,

   DMG_OP_LD_D_B       = 0x50,
   DMG_OP_LD_D_C       = 0x51,
   DMG_OP_LD_D_D       = 0x52,
   DMG_OP_LD_D_E       = 0x53,
   DMG_OP_LD_D_H       = 0x54,
   DMG_OP_LD_D_L       = 0x55,
   DMG_OP_LD_D_HL      = 0x56,
   DMG_OP_LD_D_A       = 0x57,

   DMG_OP_LD_E_B       = 0x58,
   DMG_OP_LD_E_C       = 0x59,
   DMG_OP_LD_E_D       = 0x5A,
   DMG_OP_LD_E_E       = 0x5B,
   DMG_OP_LD_E_H       = 0x5C,
   DMG_OP_LD_E_L       = 0x5D,
   DMG_OP_LD_E_HL      = 0x5E,
   DMG_OP_LD_E_A       = 0x5F,

   DMG_OP_LD_H_B       = 0x60,
   DMG_OP_LD_H_C       = 0x61,
   DMG_OP_LD_H_D       = 0x62,
   DMG_OP_LD_H_E       = 0x63,
   DMG_OP_LD_H_H       = 0x64,
   DMG_OP_LD_H_L       = 0x65,
   DMG_OP_LD_H_HL      = 0x66,
   DMG_OP_LD_H_A       = 0x67,

   DMG_OP_LD_L_B       = 0x68,
   DMG_OP_LD_L_C       = 0x69,
   DMG_OP_LD_L_D       = 0x6A,
   DMG_OP_LD_L_E       = 0x6B,
   DMG_OP_LD_L_H       = 0x6C,
   DMG_OP_LD_L_L       = 0x6D,
   DMG_OP_LD_L_HL      = 0x6E,
   DMG_OP_LD_L_A       = 0x6F,

   DMG_OP_LD_HL_B      = 0x70,
   DMG_OP_LD_HL_C      = 0x71,
   DMG_OP_LD_HL_D      = 0x72,
   DMG_OP_LD_HL_E      = 0x73,
   DMG_OP_LD_HL_H      = 0x74,
   DMG_OP_LD_HL_L      = 0x75,
   DMG_OP_LD_HL_A      = 0x77,

   DMG_OP_LD_A_B       = 0x78,
   DMG_OP_LD_A_C       = 0x79,
   DMG_OP_LD_A_D       = 0x7A,
   DMG_OP_LD_A_E       = 0x7B,
   DMG_OP_LD_A_H       = 0x7C,
   DMG_OP_LD_A_L       = 0x7D,
   DMG_OP_LD_A_HL      = 0x7E,
   DMG_OP_LD_A_A       = 0x7F,

   DMG_OP_LDH_n_A      = 0xE0,
   DMG_OP_LDH_A_n      = 0xF0,

   DMG_OP_LD_iC_A      = 0xE2,
   DMG_OP_LD_A_iC      = 0xF2,

   DMG_OP_LD_n16_A     = 0xEA,
   DMG_OP_LD_A_n16     = 0xFA,

   //8-bit Arithmetic/Logic
   DMG_OP_INC_B        = 0x04,
   DMG_OP_DEC_B        = 0x05,

   DMG_OP_INC_D        = 0x14,
   DMG_OP_DEC_D        = 0x15,

   DMG_OP_INC_H        = 0x24,
   DMG_OP_DEC_H        = 0x25,

   DMG_OP_INC_HLi      = 0x34,
   DMG_OP_DEC_HLi      = 0x35,

   DMG_OP_INC_C        = 0x0C,
   DMG_OP_DEC_C        = 0x0D,

   DMG_OP_INC_E        = 0x1C,
   DMG_OP_DEC_E        = 0x1D,

   DMG_OP_INC_L        = 0x2C,
   DMG_OP_DEC_L        = 0x2D,

   DMG_OP_INC_A        = 0x3C,
   DMG_OP_DEC_A        = 0x3D,

   DMG_OP_DAA          = 0x27,
   DMG_OP_SCF          = 0x37,
   DMG_OP_CPL          = 0x2F,
   DMG_OP_CCF          = 0x3F,
   
   DMG_OP_ADD_B        = 0x80,
   DMG_OP_ADD_C        = 0x81,
   DMG_OP_ADD_D        = 0x82,
   DMG_OP_ADD_E        = 0x83,
   DMG_OP_ADD_H        = 0x84,
   DMG_OP_ADD_L        = 0x85,
   DMG_OP_ADD_HL       = 0x86,
   DMG_OP_ADD_A        = 0x87,

   DMG_OP_ADC_B        = 0x88,
   DMG_OP_ADC_C        = 0x89,
   DMG_OP_ADC_D        = 0x8A,
   DMG_OP_ADC_E        = 0x8B,
   DMG_OP_ADC_H        = 0x8C,
   DMG_OP_ADC_L        = 0x8D,
   DMG_OP_ADC_HL       = 0x8E,
   DMG_OP_ADC_A        = 0x8F,
   
   DMG_OP_SUB_B        = 0x90,
   DMG_OP_SUB_C        = 0x91,
   DMG_OP_SUB_D        = 0x92,
   DMG_OP_SUB_E        = 0x93,
   DMG_OP_SUB_H        = 0x94,
   DMG_OP_SUB_L        = 0x95,
   DMG_OP_SUB_HL       = 0x96,
   DMG_OP_SUB_A        = 0x97,

   DMG_OP_SBC_B        = 0x98,
   DMG_OP_SBC_C        = 0x99,
   DMG_OP_SBC_D        = 0x9A,
   DMG_OP_SBC_E        = 0x9B,
   DMG_OP_SBC_H        = 0x9C,
   DMG_OP_SBC_L        = 0x9D,
   DMG_OP_SBC_HL       = 0x9E,
   DMG_OP_SBC_A        = 0x9F,
   
   DMG_OP_AND_B        = 0xA0,
   DMG_OP_AND_C        = 0xA1,
   DMG_OP_AND_D        = 0xA2,
   DMG_OP_AND_E        = 0xA3,
   DMG_OP_AND_H        = 0xA4,
   DMG_OP_AND_L        = 0xA5,
   DMG_OP_AND_HL       = 0xA6,
   DMG_OP_AND_A        = 0xA7,

   DMG_OP_XOR_B        = 0xA8,
   DMG_OP_XOR_C        = 0xA9,
   DMG_OP_XOR_D        = 0xAA,
   DMG_OP_XOR_E        = 0xAB,
   DMG_OP_XOR_H        = 0xAC,
   DMG_OP_XOR_L        = 0xAD,
   DMG_OP_XOR_HL       = 0xAE,
   DMG_OP_XOR_A        = 0xAF,
   
   DMG_OP_OR_B         = 0xB0,
   DMG_OP_OR_C         = 0xB1,
   DMG_OP_OR_D         = 0xB2,
   DMG_OP_OR_E         = 0xB3,
   DMG_OP_OR_H         = 0xB4,
   DMG_OP_OR_L         = 0xB5,
   DMG_OP_OR_HL        = 0xB6,
   DMG_OP_OR_A         = 0xB7,

   DMG_OP_CP_B         = 0xB8,
   DMG_OP_CP_C         = 0xB9,
   DMG_OP_CP_D         = 0xBA,
   DMG_OP_CP_E         = 0xBB,
   DMG_OP_CP_H         = 0xBC,
   DMG_OP_CP_L         = 0xBD,
   DMG_OP_CP_HL        = 0xBE,
   DMG_OP_CP_A         = 0xBF,

   DMG_OP_ADD_n        = 0xC6,
   DMG_OP_ADC_n        = 0xCE,
   DMG_OP_SUB_n        = 0xD6,
   DMG_OP_SBC_n        = 0xDE,
   DMG_OP_AND_n        = 0xE6,
   DMG_OP_XOR_n        = 0xEE,
   DMG_OP_OR_n         = 0xF6,
   DMG_OP_CP_n         = 0xFE,

   //16-bit Loads
   DMG_OP_LD_BC_nn     = 0x01,
   DMG_OP_LD_DE_nn     = 0x11,
   DMG_OP_LD_HL_nn     = 0x21,
   DMG_OP_LD_SP_nn     = 0x31,

   DMG_OP_LD_n16_SP    = 0x08,

   DMG_OP_POP_BC       = 0xC1,
   DMG_OP_PUSH_BC      = 0xC5,

   DMG_OP_POP_DE       = 0xD1,
   DMG_OP_PUSH_DE      = 0xD5,

   DMG_OP_POP_HL       = 0xE1,
   DMG_OP_PUSH_HL      = 0xE5,

   DMG_OP_POP_AF       = 0xF1,
   DMG_OP_PUSH_AF      = 0xF5,

   DMG_OP_LD_HL_SP_r8  = 0xF8,
   DMG_OP_LD_SP_HL     = 0xF9,

   //16-bit Arithmetic/Logical
   DMG_OP_INC_BC       = 0x03,
   DMG_OP_DEC_BC       = 0x0B,

   DMG_OP_INC_DE       = 0x13,
   DMG_OP_DEC_DE       = 0x1B,

   DMG_OP_INC_HL       = 0x23,
   DMG_OP_DEC_HL       = 0x2B,

   DMG_OP_INC_SP       = 0x33,
   DMG_OP_DEC_SP       = 0x3B,

   DMG_OP_ADD_HL_BC    = 0x09,
   DMG_OP_ADD_HL_DE    = 0x19,
   DMG_OP_ADD_HL_HL    = 0x29,
   DMG_OP_ADD_HL_SP    = 0x39,

   DMG_OP_ADD_SP_e     = 0xE8,

   //Rotates, Shifts, and Bit Operations
   DMG_OP_RLCA         = 0x07,
   DMG_OP_RRCA         = 0x0F,
   DMG_OP_RLA          = 0x17,
   DMG_OP_RRA          = 0x1F,
   DMG_OP_CB_op        = 0xCB,

   //Miscellaneous
   DMG_OP_NOP          = 0x00,
   DMG_OP_STOP         = 0x10,
   DMG_OP_HALT         = 0x76,
   DMG_OP_DI           = 0xF3,
   DMG_OP_EI           = 0xFB,

   //Undefined
   DMG_OP_UNDEFINED_0  = 0xD3,
   DMG_OP_UNDEFINED_1  = 0xDB,
   DMG_OP_UNDEFINED_2  = 0xDD,
   DMG_OP_UNDEFINED_3  = 0xE3,
   DMG_OP_UNDEFINED_4  = 0xE4,
   DMG_OP_UNDEFINED_5  = 0xEB,
   DMG_OP_UNDEFINED_6  = 0xEC,
   DMG_OP_UNDEFINED_7  = 0xED,
   DMG_OP_UNDEFINED_8  = 0xF4,
   DMG_OP_UNDEFINED_9  = 0xFC,
   DMG_OP_UNDEFINED_A  = 0xFD,
} DMG_OpCode;

void DMG_FetchInstruction(DMG_CPU* cpu);

void DMG_ExecuteInstruction(DMG_CPU* cpu);

#endif//DMG_CPU_H
