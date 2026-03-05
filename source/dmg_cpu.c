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

#include "demigod.h"

static inline Uint8 DMG_Add8(DMG_CPU* cpu, Uint8 augend, Uint8 addend);

static inline Uint8 DMG_AddWithCarry8(DMG_CPU* cpu, Uint8 augend, Uint8 addend);

static inline Uint16 DMG_Add16(DMG_CPU* cpu, Uint16 augend, Uint16 addend);

static inline Uint8 DMG_Subtract8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend);

static inline Uint8 DMG_SubtractWithCarry8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend);

static inline Uint8 DMG_And(DMG_CPU* cpu, Uint8 left, Uint8 right);

static inline Uint8 DMG_Xor(DMG_CPU* cpu, Uint8 left, Uint8 right);

static inline Uint8 DMG_Or(DMG_CPU* cpu, Uint8 left, Uint8 right);

static inline Uint8 DMG_ShiftLeft(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_RotateLeft(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_RotateLeftCircular(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_ShiftRight(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_RotateRight(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_RotateRightCircular(DMG_CPU* cpu, Uint8 value);

static inline Uint8 DMG_SwapNibbles(DMG_CPU* cpu, Uint8 value);

static inline void DMG_CheckBit(DMG_CPU* cpu, Uint8 bit, Uint8 value);

static inline void DMG_ExecuteCBOpCode(DMG_CPU* cpu, DMG_CBOpCode cb_op_code);

static inline void DMG_WriteProgramCounterToStack(DMG_CPU* cpu);

void DMG_FetchInstruction(DMG_CPU* cpu) {
   if (cpu) {
      cpu->registers.instruction = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
   }
}

void DMG_ExecuteInstruction(DMG_CPU* cpu) {
   if (!cpu)
      return;

   switch(cpu->registers.instruction) {
      case DMG_OP_JR_e: {
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.program_counter += offset;
      } break;
      case DMG_OP_JR_Z_e: {
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            cpu->registers.program_counter += offset;
         }
      } break;
      case DMG_OP_JR_C_e: {
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         if (cpu->registers.flags & DMG_CPU_FLAG_CARRY) {
            cpu->registers.program_counter += offset;
         }
      } break;
      case DMG_OP_JR_NZ_e: {
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         if (!(cpu->registers.flags & DMG_CPU_FLAG_ZERO)) {
            cpu->registers.program_counter += offset;
         }
      } break;
      case DMG_OP_JR_NC_e: {
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         if (!(cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            cpu->registers.program_counter += offset;
         }
      } break;
      case DMG_OP_JP_HL: {
         cpu->registers.program_counter = cpu->registers.hl;
      } break;
      case DMG_OP_JP_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.program_counter = SDL_Swap16LE(nn_more_significant | (nn_less_significant << 8));
      } break;
      case DMG_OP_JP_Z_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         
         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            cpu->registers.program_counter = SDL_Swap16LE(nn_more_significant | (nn_less_significant << 8));
         }
      } break;
      case DMG_OP_JP_C_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         
         if (cpu->registers.flags & DMG_CPU_FLAG_CARRY) {
            cpu->registers.program_counter = SDL_Swap16LE(nn_more_significant | (nn_less_significant << 8));
         }
      } break;
      case DMG_OP_JP_NZ_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         
         if (!(cpu->registers.flags & DMG_CPU_FLAG_ZERO)) {
            cpu->registers.program_counter = SDL_Swap16LE(nn_more_significant | (nn_less_significant << 8));
         }
      } break;
      case DMG_OP_JP_NC_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         
         if (!(cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            cpu->registers.program_counter = SDL_Swap16LE(nn_more_significant | (nn_less_significant << 8));
         }
      } break;
      case DMG_OP_CALL_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
      } break;
      case DMG_OP_CALL_Z_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_C_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (cpu->registers.flags & DMG_CPU_FLAG_CARRY) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_NZ_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (!(cpu->registers.flags & DMG_CPU_FLAG_ZERO)) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_NC_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (!(cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         }
      } break;
      case DMG_OP_RET: {
         Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
      } break;
      case DMG_OP_RET_Z: {
         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
         }
      } break;
      case DMG_OP_RET_C: {
         if (cpu->registers.flags & DMG_CPU_FLAG_CARRY) {
            Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
         }
      } break;
      case DMG_OP_RET_NZ: {
         if (!(cpu->registers.flags & DMG_CPU_FLAG_ZERO)) {
            Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
         }
      } break;
      case DMG_OP_RET_NC: {
         if (!(cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
            cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
         }
      } break;
      case DMG_OP_RETI: {
         Uint8 return_address_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 return_address_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.program_counter = DMG_ComposeUint16(return_address_less_significant, return_address_more_significant);
         cpu->registers.interrupt_enable = 1;
      } break;
      case DMG_OP_RST_00: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x00, 0x00);
      } break;
      case DMG_OP_RST_10: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x10, 0x00);
      } break;
      case DMG_OP_RST_20: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x20, 0x00);
      } break;
      case DMG_OP_RST_30: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x30, 0x00);
      } break;
      case DMG_OP_RST_08: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x08, 0x00);
      } break;
      case DMG_OP_RST_18: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x18, 0x00);
      } break;
      case DMG_OP_RST_28: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x28, 0x00);
      } break;
      case DMG_OP_RST_38: {
         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(0x38, 0x00);
      } break;
      case DMG_OP_LD_BC_A: {
         DMG_WriteMemory(cpu, cpu->registers.bc, cpu->registers.a);
      } break;
      case DMG_OP_LD_DE_A: {
         DMG_WriteMemory(cpu, cpu->registers.de, cpu->registers.a);
      } break;
      case DMG_OP_LD_HLi_A: {
         DMG_WriteMemory(cpu, cpu->registers.hl++, cpu->registers.a);
      } break;
      case DMG_OP_LD_HLd_A: {
         DMG_WriteMemory(cpu, cpu->registers.hl--, cpu->registers.a);
      } break;
      case DMG_OP_LD_B_n: {
         cpu->registers.b = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_D_n: {
         cpu->registers.d = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_H_n: {
         cpu->registers.h = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_HL_n: {
         Uint8 n = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         DMG_WriteMemory(cpu, cpu->registers.hl, n);
      } break;
      case DMG_OP_LD_C_n: {
         cpu->registers.c = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_E_n: {
         cpu->registers.e = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_L_n: {
         cpu->registers.l = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_A_n: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
      } break;
      case DMG_OP_LD_A_BC: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.bc);
      } break;
      case DMG_OP_LD_A_DE: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.de);
      } break;
      case DMG_OP_LD_A_HLi: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.hl++);
      } break;
      case DMG_OP_LD_A_HLd: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.hl--);
      } break;
      case DMG_OP_LD_B_B: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_B_C: {
         cpu->registers.b = cpu->registers.c;
      } break;
      case DMG_OP_LD_B_D: {
         cpu->registers.b = cpu->registers.d;
      } break;
      case DMG_OP_LD_B_E: {
         cpu->registers.b = cpu->registers.e;
      } break;
      case DMG_OP_LD_B_H: {
         cpu->registers.b = cpu->registers.h;
      } break;
      case DMG_OP_LD_B_L: {
         cpu->registers.b = cpu->registers.l;
      } break;
      case DMG_OP_LD_B_HL: {
         cpu->registers.b = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_B_A: {
         cpu->registers.b = cpu->registers.a;
      } break;
      case DMG_OP_LD_C_B: {
         cpu->registers.c = cpu->registers.b;
      } break;
      case DMG_OP_LD_C_C: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_C_D: {
         cpu->registers.c = cpu->registers.d;
      } break;
      case DMG_OP_LD_C_E: {
         cpu->registers.c = cpu->registers.e;
      } break;
      case DMG_OP_LD_C_H: {
         cpu->registers.c = cpu->registers.h;
      } break;
      case DMG_OP_LD_C_L: {
         cpu->registers.c = cpu->registers.l;
      } break;
      case DMG_OP_LD_C_HL: {
         cpu->registers.c = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_C_A: {
         cpu->registers.c = cpu->registers.a;
      } break;
      case DMG_OP_LD_D_B: {
         cpu->registers.d = cpu->registers.b;
      } break;
      case DMG_OP_LD_D_C: {
         cpu->registers.d = cpu->registers.c;
      } break;
      case DMG_OP_LD_D_D: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_D_E: {
         cpu->registers.d = cpu->registers.e;
      } break;
      case DMG_OP_LD_D_H: {
         cpu->registers.d = cpu->registers.h;
      } break;
      case DMG_OP_LD_D_L: {
         cpu->registers.d = cpu->registers.l;
      } break;
      case DMG_OP_LD_D_HL: {
         cpu->registers.d = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_D_A: {
         cpu->registers.d = cpu->registers.a;
      } break;
      case DMG_OP_LD_E_B: {
         cpu->registers.e = cpu->registers.b;
      } break;
      case DMG_OP_LD_E_C: {
         cpu->registers.e = cpu->registers.c;
      } break;
      case DMG_OP_LD_E_D: {
         cpu->registers.e = cpu->registers.d;
      } break;
      case DMG_OP_LD_E_E: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_E_H: {
         cpu->registers.e = cpu->registers.h;
      } break;
      case DMG_OP_LD_E_L: {
         cpu->registers.e = cpu->registers.l;
      } break;
      case DMG_OP_LD_E_HL: {
         cpu->registers.e = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_E_A: {
         cpu->registers.e = cpu->registers.a;
      } break;
      case DMG_OP_LD_H_B: {
         cpu->registers.h = cpu->registers.b;
      } break;
      case DMG_OP_LD_H_C: {
         cpu->registers.h = cpu->registers.c;
      } break;
      case DMG_OP_LD_H_D: {
         cpu->registers.h = cpu->registers.d;
      } break;
      case DMG_OP_LD_H_E: {
         cpu->registers.h = cpu->registers.e;
      } break;
      case DMG_OP_LD_H_H: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_H_L: {
         cpu->registers.h = cpu->registers.l;
      } break;
      case DMG_OP_LD_H_HL: {
         cpu->registers.h = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_H_A: {
         cpu->registers.h = cpu->registers.a;
      } break;
      case DMG_OP_LD_L_B: {
         cpu->registers.l = cpu->registers.b;
      } break;
      case DMG_OP_LD_L_C: {
         cpu->registers.l = cpu->registers.c;
      } break;
      case DMG_OP_LD_L_D: {
         cpu->registers.l = cpu->registers.d;
      } break;
      case DMG_OP_LD_L_E: {
         cpu->registers.l = cpu->registers.e;
      } break;
      case DMG_OP_LD_L_H: {
         cpu->registers.l = cpu->registers.h;
      } break;
      case DMG_OP_LD_L_L: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LD_L_HL: {
         cpu->registers.l = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_L_A: {
         cpu->registers.l = cpu->registers.a;
      } break;
      case DMG_OP_LD_HL_B: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.b);
      } break;
      case DMG_OP_LD_HL_C: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.c);
      } break;
      case DMG_OP_LD_HL_D: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.d);
      } break;
      case DMG_OP_LD_HL_E: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.e);
      } break;
      case DMG_OP_LD_HL_H: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.h);
      } break;
      case DMG_OP_LD_HL_L: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.l);
      } break;
      case DMG_OP_LD_HL_A: {
         DMG_WriteMemory(cpu, cpu->registers.hl, cpu->registers.a);
      } break;
      case DMG_OP_LD_A_B: {
         cpu->registers.a = cpu->registers.b;
      } break;
      case DMG_OP_LD_A_C: {
         cpu->registers.a = cpu->registers.c;
      } break;
      case DMG_OP_LD_A_D: {
         cpu->registers.a = cpu->registers.d;
      } break;
      case DMG_OP_LD_A_E: {
         cpu->registers.a = cpu->registers.e;
      } break;
      case DMG_OP_LD_A_H: {
         cpu->registers.a = cpu->registers.h;
      } break;
      case DMG_OP_LD_A_L: {
         cpu->registers.a = cpu->registers.l;
      } break;
      case DMG_OP_LD_A_HL: {
         cpu->registers.a = DMG_ReadMemory(cpu, cpu->registers.hl);
      } break;
      case DMG_OP_LD_A_A: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_LDH_n_A: {
         Uint8 n = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         DMG_WriteMemory(cpu, DMG_ComposeUint16(n, 0xFF), cpu->registers.a);
      } break;
      case DMG_OP_LDH_A_n: {
         Uint8 n = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_ReadMemory(cpu, n);
      } break;
      case DMG_OP_LD_iC_A: {
         DMG_WriteMemory(cpu, DMG_ComposeUint16(cpu->registers.c, 0xFF), cpu->registers.a);
      } break;
      case DMG_OP_LD_A_iC: {
         cpu->registers.a = DMG_ReadMemory(cpu, DMG_ComposeUint16(cpu->registers.c, 0xFF));
      } break;
      case DMG_OP_LD_n16_A: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint16 nn = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         DMG_WriteMemory(cpu, nn, cpu->registers.a);
      } break;
      case DMG_OP_LD_A_n16: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint16 nn = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
         cpu->registers.a = DMG_ReadMemory(cpu, nn);
      } break;
      case DMG_OP_INC_B: {
         cpu->registers.b = DMG_Add8(cpu, cpu->registers.b, 1);
      } break;
      case DMG_OP_DEC_B: {
         cpu->registers.b = DMG_Subtract8(cpu, cpu->registers.b, 1);
      } break;
      case DMG_OP_INC_D: {
         cpu->registers.d = DMG_Add8(cpu, cpu->registers.d, 1);
      } break;
      case DMG_OP_DEC_D: {
         cpu->registers.d = DMG_Subtract8(cpu, cpu->registers.d, 1);
      } break;
      case DMG_OP_INC_H: {
         cpu->registers.h = DMG_Add8(cpu, cpu->registers.h, 1);
      } break;
      case DMG_OP_DEC_H: {
         cpu->registers.h = DMG_Subtract8(cpu, cpu->registers.h, 1);
      } break;
      case DMG_OP_INC_HLi: {
         Uint8 hl_value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_WriteMemory(cpu, cpu->registers.hl, DMG_Add8(cpu, hl_value, 1));
      } break;
      case DMG_OP_DEC_HLi: {
         Uint8 hl_value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_WriteMemory(cpu, cpu->registers.hl, DMG_Subtract8(cpu, hl_value, 1));
      } break;
      case DMG_OP_INC_C: {
         cpu->registers.c = DMG_Add8(cpu, cpu->registers.c, 1);
      } break;
      case DMG_OP_DEC_C: {
         cpu->registers.c = DMG_Subtract8(cpu, cpu->registers.c, 1);
      } break;
      case DMG_OP_INC_E: {
         cpu->registers.e = DMG_Add8(cpu, cpu->registers.e, 1);
      } break;
      case DMG_OP_DEC_E: {
         cpu->registers.e = DMG_Subtract8(cpu, cpu->registers.e, 1);
      } break;
      case DMG_OP_INC_L: {
         cpu->registers.l = DMG_Add8(cpu, cpu->registers.l, 1);
      } break;
      case DMG_OP_DEC_L: {
         cpu->registers.l = DMG_Subtract8(cpu, cpu->registers.l, 1);
      } break;
      case DMG_OP_INC_A: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.e, 1);
      } break;
      case DMG_OP_DEC_A: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, 1);
      } break;
      case DMG_OP_DAA: {
         Uint8 offset = 0x00;
         if ((cpu->registers.a & 0xF) > 0x9 || cpu->registers.flags & DMG_CPU_FLAG_HALF_CARRY) {
            offset |= 0x06;
         }
         if ((cpu->registers.a > 0x99) || (cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            offset |= 0x60;
         }
         cpu->registers.a = (Uint8)(((Uint16)cpu->registers.a + offset) % 255);
      } break;
      case DMG_OP_SCF: {
         cpu->registers.flags &= ~DMG_CPU_FLAG_SUBTRACTION;
         cpu->registers.flags &= ~DMG_CPU_FLAG_HALF_CARRY;
         cpu->registers.flags |= DMG_CPU_FLAG_CARRY;
      } break;
      case DMG_OP_CPL: {
         cpu->registers.a = ~cpu->registers.a;

         cpu->registers.flags |= DMG_CPU_FLAG_SUBTRACTION | DMG_CPU_FLAG_HALF_CARRY;
      } break;
      case DMG_OP_CCF: {
         DMG_CPUFlags old_flags = cpu->registers.flags;

         cpu->registers.flags &= ~DMG_CPU_FLAG_SUBTRACTION;
         cpu->registers.flags &= ~DMG_CPU_FLAG_HALF_CARRY;
         cpu->registers.flags &= ~DMG_CPU_FLAG_CARRY;
         cpu->registers.flags |= ~old_flags & DMG_CPU_FLAG_CARRY;
      } break;
      case DMG_OP_ADD_B: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_ADD_C: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_ADD_D: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_ADD_E: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_ADD_H: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_ADD_L: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_ADD_HL: {
         Uint8 addend = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, addend);
      } break;
      case DMG_OP_ADD_A: {
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_ADC_B: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_ADC_C: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_ADC_D: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_ADC_E: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_ADC_H: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_ADC_L: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_ADC_HL: {
         Uint8 addend = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, addend);
      } break;
      case DMG_OP_ADC_A: {
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_SUB_B: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_SUB_C: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_SUB_D: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_SUB_E: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_SUB_H: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_SUB_L: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_SUB_HL: {
         Uint8 subtrahend = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, subtrahend);
      } break;
      case DMG_OP_SUB_A: {
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_SBC_B: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_SBC_C: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_SBC_D: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_SBC_E: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_SBC_H: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_SBC_L: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_SBC_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_SBC_A: {
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_AND_B: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_AND_C: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_AND_D: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_AND_E: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_AND_H: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_AND_L: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_AND_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_AND_A: {
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_XOR_B: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_XOR_C: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_XOR_D: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_XOR_E: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_XOR_H: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_XOR_L: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_XOR_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_XOR_A: {
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_OR_B: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_OR_C: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_OR_D: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_OR_E: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_OR_H: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_OR_L: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_OR_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_OR_A: {
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_CP_B: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.b);
      } break;
      case DMG_OP_CP_C: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.c);
      } break;
      case DMG_OP_CP_D: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.d);
      } break;
      case DMG_OP_CP_E: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.e);
      } break;
      case DMG_OP_CP_H: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.h);
      } break;
      case DMG_OP_CP_L: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.l);
      } break;
      case DMG_OP_CP_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_Subtract8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_CP_A: {
         DMG_Subtract8(cpu, cpu->registers.a, cpu->registers.a);
      } break;
      case DMG_OP_ADD_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_Add8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_ADC_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_AddWithCarry8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_SUB_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_Subtract8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_SBC_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_SubtractWithCarry8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_AND_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_And(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_XOR_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_Xor(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_OR_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.a = DMG_Or(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_CP_n: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         DMG_Subtract8(cpu, cpu->registers.a, value);
      } break;
      case DMG_OP_LD_BC_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.bc = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
      } break;
      case DMG_OP_LD_DE_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.de = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
      } break;
      case DMG_OP_LD_HL_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.hl = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
      } break;
      case DMG_OP_LD_SP_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.sp = DMG_ComposeUint16(nn_less_significant, nn_more_significant);
      } break;
      case DMG_OP_LD_n16_SP: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint16 nn = DMG_ComposeUint16(nn_less_significant, nn_more_significant);

         Uint8 sp_less_significant;
         Uint8 sp_more_significant;
         DMG_DecomposeUint16(cpu->registers.sp, &sp_less_significant, &sp_more_significant);

         DMG_WriteMemory(cpu, nn, sp_less_significant);
         nn++;
         DMG_WriteMemory(cpu, nn, sp_more_significant);
      } break;
      case DMG_OP_POP_BC: {
         Uint8 value_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 value_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.bc = DMG_ComposeUint16(value_less_significant, value_more_significant);
      } break;
      case DMG_OP_PUSH_BC: {
         Uint8 bc_less_significant;
         Uint8 bc_more_significant;
         DMG_DecomposeUint16(cpu->registers.bc, &bc_less_significant, &bc_more_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, bc_less_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, bc_more_significant);
      } break;
      case DMG_OP_POP_DE: {
         Uint8 value_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 value_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.de = DMG_ComposeUint16(value_less_significant, value_more_significant);
      } break;
      case DMG_OP_PUSH_DE: {
         Uint8 de_less_significant;
         Uint8 de_more_significant;
         DMG_DecomposeUint16(cpu->registers.de, &de_less_significant, &de_more_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, de_less_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, de_more_significant);
      } break;
      case DMG_OP_POP_HL: {
         Uint8 value_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 value_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.hl = DMG_ComposeUint16(value_less_significant, value_more_significant);
      } break;
      case DMG_OP_PUSH_HL: {
         Uint8 hl_less_significant;
         Uint8 hl_more_significant;
         DMG_DecomposeUint16(cpu->registers.hl, &hl_less_significant, &hl_more_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, hl_less_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, hl_more_significant);
      } break;
      case DMG_OP_POP_AF: {
         Uint8 value_less_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         Uint8 value_more_significant = DMG_ReadMemory(cpu, cpu->registers.stack_pointer++);
         cpu->registers.af = DMG_ComposeUint16(value_less_significant, value_more_significant);
      } break;
      case DMG_OP_PUSH_AF: {
         Uint8 af_less_significant;
         Uint8 af_more_significant;
         DMG_DecomposeUint16(cpu->registers.af, &af_less_significant, &af_more_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, af_less_significant);

         cpu->registers.stack_pointer--;
         DMG_WriteMemory(cpu, cpu->registers.stack_pointer, af_more_significant);
      } break;
      case DMG_OP_LD_HL_SP_r8: {
         Uint8 unsigned_offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Sint8 offset = *((Sint8*)&unsigned_offset);

         {
            /* calculate flags */
            Uint8 sp_less_significant;
            DMG_DecomposeUint16(cpu->registers.stack_pointer, &sp_less_significant, nullptr);
            DMG_Add8(cpu, sp_less_significant, offset);
            cpu->registers.flags &= ~DMG_CPU_FLAG_ZERO;
         }

         cpu->registers.hl = DMG_ReadMemory(cpu, cpu->registers.stack_pointer + offset);
      } break;
      case DMG_OP_LD_SP_HL: {
         cpu->registers.sp = cpu->registers.hl;
      } break;
      case DMG_OP_INC_BC: {
         cpu->registers.bc++;
      } break;
      case DMG_OP_DEC_BC: {
         cpu->registers.bc--;
      } break;
      case DMG_OP_INC_DE: {
         cpu->registers.de++;
      } break;
      case DMG_OP_DEC_DE: {
         cpu->registers.de--;
      } break;
      case DMG_OP_INC_HL: {
         cpu->registers.hl++;
      } break;
      case DMG_OP_DEC_HL: {
         cpu->registers.hl--;
      } break;
      case DMG_OP_INC_SP: {
         cpu->registers.sp++;
      } break;
      case DMG_OP_DEC_SP: {
         cpu->registers.sp--;
      } break;
      case DMG_OP_ADD_HL_BC: {
         cpu->registers.hl = DMG_Add16(cpu, cpu->registers.hl, cpu->registers.bc);
      } break;
      case DMG_OP_ADD_HL_DE: {
         cpu->registers.hl = DMG_Add16(cpu, cpu->registers.hl, cpu->registers.de);
      } break;
      case DMG_OP_ADD_HL_HL: {
         cpu->registers.hl = DMG_Add16(cpu, cpu->registers.hl, cpu->registers.hl);
      } break;
      case DMG_OP_ADD_HL_SP: {
         cpu->registers.hl = DMG_Add16(cpu, cpu->registers.hl, cpu->registers.sp);
      } break;
      case DMG_OP_ADD_SP_e: {
         /* This is technically signed but that doesn't matter, the add routine will handle that */
         Uint16 offset = (Uint16)DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.stack_pointer = DMG_Add16(cpu, cpu->registers.sp, offset);
      } break;
      case DMG_OP_RLCA: {
         cpu->registers.a = DMG_RotateLeftCircular(cpu, cpu->registers.a);
      } break;
      case DMG_OP_RRCA: {
         cpu->registers.a = DMG_RotateRightCircular(cpu, cpu->registers.a);
      } break;
      case DMG_OP_RLA: {
         cpu->registers.a = DMG_RotateLeft(cpu, cpu->registers.a);
      } break;
      case DMG_OP_RRA: {
         cpu->registers.a = DMG_RotateRight(cpu, cpu->registers.a);
      } break;
      case DMG_OP_CB_op: {
         /* Note:
          * Realistically this doesn't need to be another function because it's only called here.
          * However, I don't particularly want a 256+ line case statement so I've broken it out.
          *    - remi 05 Mar 26
          */
         DMG_CBOpCode cb_op_code = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         DMG_ExecuteCBOpCode(cpu, cb_op_code);
      } break;
      case DMG_OP_NOP: {
         /* NO-OPERATION */
      } break;
      case DMG_OP_STOP: {
         /* TODO: implement */
      } break;
      case DMG_OP_HALT: {
         /* TODO: implement */
      } break;
      case DMG_OP_DI: {
         cpu->next_cycle_interrupt_change = 0;
      } break;
      case DMG_OP_EI: {
         cpu->next_cycle_interrupt_change = 0;
      } break;

      /*
       * Note remi, 4 March 2026
       * Should these act as a NOP?
       * Or should the program terminate?
       * I don't know.
       * Further research is required.
       */
      case DMG_OP_UNDEFINED_0: {
      } break;
      case DMG_OP_UNDEFINED_1: {
      } break;
      case DMG_OP_UNDEFINED_2: {
      } break;
      case DMG_OP_UNDEFINED_3: {
      } break;
      case DMG_OP_UNDEFINED_4: {
      } break;
      case DMG_OP_UNDEFINED_5: {
      } break;
      case DMG_OP_UNDEFINED_6: {
      } break;
      case DMG_OP_UNDEFINED_7: {
      } break;
      case DMG_OP_UNDEFINED_8: {
      } break;
      case DMG_OP_UNDEFINED_9: {
      } break;
      case DMG_OP_UNDEFINED_A: {
      } break;
   }
}

static inline Uint8 DMG_Add8(DMG_CPU* cpu, Uint8 augend, Uint8 addend) {
   cpu->registers.flags &= ~DMG_CPU_FLAG_CARRY;
   return DMG_AddWithCarry8(cpu, augend, addend);
}

static inline Uint8 DMG_AddWithCarry8(DMG_CPU* cpu, Uint8 augend, Uint8 addend) {
   Uint8 carry_in = (cpu->registers.flags & DMG_CPU_FLAG_CARRY) >> DMG_CPU_FLAG_CARRY_SHIFT;
   Uint8 result = (Uint8)(((Uint16)augend + (Uint16)addend + carry_in) % 255);

   Uint8 half_carry = (((augend & 0xF) + (addend & 0xF) + carry_in) > 0xF) << DMG_CPU_FLAG_HALF_CARRY_SHIFT;

   /* If a full carry occured, bit 9 will be 1; shift it by 9 - CARRY_SHIFT (3) = 6 */
   Uint8 full_carry = ((Uint16)augend + (Uint16)addend + (Uint16)carry_in) >> 6;

   cpu->registers.flags = 0;
   cpu->registers.flags |= half_carry;
   cpu->registers.flags |= full_carry;
   cpu->registers.flags |= (result == 0) << DMG_CPU_FLAG_ZERO_SHIFT;

   return result;
}

static inline Uint16 DMG_Add16(DMG_CPU* cpu, Uint16 augend, Uint16 addend) {
   Uint16 result = augend + addend;

   cpu->registers.flags = 0;

   /* The Gameboy looks for the half-carry on bit 11, flag is on bit 5, so we shift right by 12 - 5 = 7 */
   cpu->registers.flags |= (Uint8)(((augend & 0b11111111111) + (addend & 0b11111111111)) >> 7);

   /* The Gameboy carry flag is on bit 4, we need to shift bit 17 into that position, so we move by 17 - 4 = 13 */
   cpu->registers.flags |= (Uint8)((((Uint32)augend + (Uint32)addend) >> 13) & 1);

   cpu->registers.flags |= (Uint8)(result == 0) << DMG_CPU_FLAG_ZERO_SHIFT;

   return result;
}

static inline Uint8 DMG_Subtract8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend) {
   cpu->registers.flags &= ~DMG_CPU_FLAG_HALF_CARRY;
   cpu->registers.flags |= (Uint8)((minuend & 0xF) < (subtrahend & 0xF)) << DMG_CPU_FLAG_HALF_CARRY_SHIFT;
   
   cpu->registers.flags &= ~DMG_CPU_FLAG_CARRY;
   cpu->registers.flags |= (Uint8)(minuend < subtrahend) << DMG_CPU_FLAG_CARRY_SHIFT;

   cpu->registers.flags |= DMG_CPU_FLAG_SUBTRACTION;

   return minuend - subtrahend;
}

static inline Uint8 DMG_SubtractWithCarry8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend) {
   subtrahend += ((cpu->registers.flags & DMG_CPU_FLAG_CARRY) >> DMG_CPU_FLAG_CARRY_SHIFT) * ((subtrahend < 0) ? -1 : 1);
   return DMG_Subtract8(cpu, minuend, subtrahend);
}

static inline Uint8 DMG_And(DMG_CPU* cpu, Uint8 left, Uint8 right) {
   Uint8 result;

   result = left & right;
   cpu->registers.flags = DMG_CPU_FLAG_HALF_CARRY | ((result == 0) << DMG_CPU_FLAG_ZERO_SHIFT);

   return result;
}

static inline Uint8 DMG_Xor(DMG_CPU* cpu, Uint8 left, Uint8 right) {
   Uint8 result;

   result = left ^ right;
   cpu->registers.flags = (result == 0) ? DMG_CPU_FLAG_ZERO : 0;

   return result;
}

static inline Uint8 DMG_Or(DMG_CPU* cpu, Uint8 left, Uint8 right) {
   Uint8 result;

   result = left | right;
   cpu->registers.flags = (result == 0) ? DMG_CPU_FLAG_ZERO : 0;

   return result;
}

static inline Uint8 DMG_ShiftLeft(DMG_CPU* cpu, Uint8 value) {
   /* We shift right by 4 because we need the seventh bit shifted to the carry flag (third bit), 7 - 3 = 4 */
   cpu->registers.flags = (value & 0b10000000) >> 4;
   return value << 1;
}

static inline Uint8 DMG_RotateLeft(DMG_CPU* cpu, Uint8 value) {
   Uint8 result = (value << 1) | ((cpu->registers.flags & DMG_CPU_FLAG_CARRY) >> DMG_CPU_FLAG_CARRY_SHIFT);
   cpu->registers.flags = (value & 0b10000000) >> 4;
   return result;
}

static inline Uint8 DMG_RotateLeftCircular(DMG_CPU* cpu, Uint8 value) {
   /* See DMG_RotateLeft */
   cpu->registers.flags = (value & 0b10000000) >> 4;
   return (value << 1) | (value >> 7);
}

static inline Uint8 DMG_ShiftRight(DMG_CPU* cpu, Uint8 value) {
   cpu->registers.flags = (value & 0b1) << DMG_CPU_FLAG_CARRY_SHIFT;
   return value >> 1;
}

static inline Uint8 DMG_RotateRight(DMG_CPU* cpu, Uint8 value) {
   /* CARRY_FLAG is bit 4; we need to shift it to bit 7. (7 - 4 = 3) */
   Uint8 result = (value >> 1) | (cpu->registers.flags & DMG_CPU_FLAG_CARRY) << 3;
   cpu->registers.flags = (value & 0b1) << DMG_CPU_FLAG_CARRY_SHIFT;
   return result;
}

static inline Uint8 DMG_RotateRightCircular(DMG_CPU* cpu, Uint8 value) {
   cpu->registers.flags = (value & 0b1) << DMG_CPU_FLAG_CARRY_SHIFT;
   return (value >> 1) | (value << 7);
}

static inline Uint8 DMG_SwapNibbles(DMG_CPU* cpu, Uint8 value) {
   Uint8 result = ((value & 0xF) << 8) | ((value & 0xF0) >> 8);
   cpu->registers.flags = (Uint8)((result == 0) & 0b1) << DMG_CPU_FLAG_ZERO_SHIFT;
   return result;
}

static inline void DMG_CheckBit(DMG_CPU* cpu, Uint8 bit, Uint8 value) {
   Uint8 zero_flag = ((value >> bit) & 0b1) << DMG_CPU_FLAG_ZERO_SHIFT;
   cpu->registers.flags = DMG_CPU_FLAG_HALF_CARRY | zero_flag;
}

static inline void DMG_ExecuteCBOpCode(DMG_CPU* cpu, DMG_CBOpCode cb_op_code) {
   switch (cb_op_code) {
      case DMG_CB_RLC_B: {
         cpu->registers.b = DMG_RotateLeftCircular(cpu, cpu->registers.b);
      } break;
      case DMG_CB_RLC_C: {
         cpu->registers.c = DMG_RotateLeftCircular(cpu, cpu->registers.c);
      } break;
      case DMG_CB_RLC_D: {
         cpu->registers.d = DMG_RotateLeftCircular(cpu, cpu->registers.d);
      } break;
      case DMG_CB_RLC_E: {
         cpu->registers.e = DMG_RotateLeftCircular(cpu, cpu->registers.e);
      } break;
      case DMG_CB_RLC_H: {
         cpu->registers.h = DMG_RotateLeftCircular(cpu, cpu->registers.h);
      } break;
      case DMG_CB_RLC_L: {
         cpu->registers.l = DMG_RotateLeftCircular(cpu, cpu->registers.l);
      } break;
      case DMG_CB_RLC_A: {
         cpu->registers.a = DMG_RotateLeftCircular(cpu, cpu->registers.a);
      } break;
      case DMG_CB_RLC_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_RotateLeftCircular(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RRC_B: {
         cpu->registers.b = DMG_RotateRightCircular(cpu, cpu->registers.b);
      } break;
      case DMG_CB_RRC_C: {
         cpu->registers.c = DMG_RotateRightCircular(cpu, cpu->registers.c);
      } break;
      case DMG_CB_RRC_D: {
         cpu->registers.d = DMG_RotateRightCircular(cpu, cpu->registers.d);
      } break;
      case DMG_CB_RRC_E: {
         cpu->registers.e = DMG_RotateRightCircular(cpu, cpu->registers.e);
      } break;
      case DMG_CB_RRC_H: {
         cpu->registers.h = DMG_RotateRightCircular(cpu, cpu->registers.h);
      } break;
      case DMG_CB_RRC_L: {
         cpu->registers.l = DMG_RotateRightCircular(cpu, cpu->registers.l);
      } break;
      case DMG_CB_RRC_A: {
         cpu->registers.a = DMG_RotateRightCircular(cpu, cpu->registers.a);
      } break;
      case DMG_CB_RRC_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_RotateRightCircular(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RL_B: {
         cpu->registers.b = DMG_RotateLeft(cpu, cpu->registers.b);
      } break;
      case DMG_CB_RL_C: {
         cpu->registers.c = DMG_RotateLeft(cpu, cpu->registers.c);
      } break;
      case DMG_CB_RL_D: {
         cpu->registers.d = DMG_RotateLeft(cpu, cpu->registers.d);
      } break;
      case DMG_CB_RL_E: {
         cpu->registers.e = DMG_RotateLeft(cpu, cpu->registers.e);
      } break;
      case DMG_CB_RL_H: {
         cpu->registers.h = DMG_RotateLeft(cpu, cpu->registers.h);
      } break;
      case DMG_CB_RL_L: {
         cpu->registers.l = DMG_RotateLeft(cpu, cpu->registers.l);
      } break;
      case DMG_CB_RL_A: {
         cpu->registers.a = DMG_RotateLeft(cpu, cpu->registers.a);
      } break;
      case DMG_CB_RL_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_RotateLeft(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RR_B: {
         cpu->registers.b = DMG_RotateRight(cpu, cpu->registers.b);
      } break;
      case DMG_CB_RR_C: {
         cpu->registers.c = DMG_RotateRight(cpu, cpu->registers.c);
      } break;
      case DMG_CB_RR_D: {
         cpu->registers.d = DMG_RotateRight(cpu, cpu->registers.d);
      } break;
      case DMG_CB_RR_E: {
         cpu->registers.e = DMG_RotateRight(cpu, cpu->registers.e);
      } break;
      case DMG_CB_RR_H: {
         cpu->registers.h = DMG_RotateRight(cpu, cpu->registers.h);
      } break;
      case DMG_CB_RR_L: {
         cpu->registers.l = DMG_RotateRight(cpu, cpu->registers.l);
      } break;
      case DMG_CB_RR_A: {
         cpu->registers.a = DMG_RotateRight(cpu, cpu->registers.a);
      } break;
      case DMG_CB_RR_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_RotateRight(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SLA_B: {
         cpu->registers.b = DMG_ShiftLeft(cpu, cpu->registers.b);
      } break;
      case DMG_CB_SLA_C: {
         cpu->registers.c = DMG_ShiftLeft(cpu, cpu->registers.c);
      } break;
      case DMG_CB_SLA_D: {
         cpu->registers.d = DMG_ShiftLeft(cpu, cpu->registers.d);
      } break;
      case DMG_CB_SLA_E: {
         cpu->registers.e = DMG_ShiftLeft(cpu, cpu->registers.e);
      } break;
      case DMG_CB_SLA_H: {
         cpu->registers.h = DMG_ShiftLeft(cpu, cpu->registers.h);
      } break;
      case DMG_CB_SLA_L: {
         cpu->registers.l = DMG_ShiftLeft(cpu, cpu->registers.l);
      } break;
      case DMG_CB_SLA_A: {
         cpu->registers.a = DMG_ShiftLeft(cpu, cpu->registers.a);
      } break;
      case DMG_CB_SLA_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_ShiftLeft(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SRA_B: {
         cpu->registers.b = DMG_ShiftRight(cpu, cpu->registers.b);
      } break;
      case DMG_CB_SRA_C: {
         cpu->registers.c = DMG_ShiftRight(cpu, cpu->registers.c);
      } break;
      case DMG_CB_SRA_D: {
         cpu->registers.d = DMG_ShiftRight(cpu, cpu->registers.d);
      } break;
      case DMG_CB_SRA_E: {
         cpu->registers.e = DMG_ShiftRight(cpu, cpu->registers.e);
      } break;
      case DMG_CB_SRA_H: {
         cpu->registers.h = DMG_ShiftRight(cpu, cpu->registers.h);
      } break;
      case DMG_CB_SRA_L: {
         cpu->registers.l = DMG_ShiftRight(cpu, cpu->registers.l);
      } break;
      case DMG_CB_SRA_A: {
         cpu->registers.a = DMG_ShiftRight(cpu, cpu->registers.a);
      } break;
      case DMG_CB_SRA_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_ShiftRight(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SWAP_B: {
         cpu->registers.b = DMG_SwapNibbles(cpu, cpu->registers.b);
      } break;
      case DMG_CB_SWAP_C: {
         cpu->registers.c = DMG_SwapNibbles(cpu, cpu->registers.c);
      } break;
      case DMG_CB_SWAP_D: {
         cpu->registers.d = DMG_SwapNibbles(cpu, cpu->registers.d);
      } break;
      case DMG_CB_SWAP_E: {
         cpu->registers.e = DMG_SwapNibbles(cpu, cpu->registers.e);
      } break;
      case DMG_CB_SWAP_H: {
         cpu->registers.h = DMG_SwapNibbles(cpu, cpu->registers.h);
      } break;
      case DMG_CB_SWAP_L: {
         cpu->registers.l = DMG_SwapNibbles(cpu, cpu->registers.l);
      } break;
      case DMG_CB_SWAP_A: {
         cpu->registers.a = DMG_SwapNibbles(cpu, cpu->registers.a);
      } break;
      case DMG_CB_SWAP_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_SwapNibbles(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SRL_B: {
         cpu->registers.b = DMG_ShiftRight(cpu, cpu->registers.b);
      } break;
      case DMG_CB_SRL_C: {
         cpu->registers.c = DMG_ShiftRight(cpu, cpu->registers.c);
      } break;
      case DMG_CB_SRL_D: {
         cpu->registers.d = DMG_ShiftRight(cpu, cpu->registers.d);
      } break;
      case DMG_CB_SRL_E: {
         cpu->registers.e = DMG_ShiftRight(cpu, cpu->registers.e);
      } break;
      case DMG_CB_SRL_H: {
         cpu->registers.h = DMG_ShiftRight(cpu, cpu->registers.h);
      } break;
      case DMG_CB_SRL_L: {
         cpu->registers.l = DMG_ShiftRight(cpu, cpu->registers.l);
      } break;
      case DMG_CB_SRL_A: {
         cpu->registers.a = DMG_ShiftRight(cpu, cpu->registers.a);
      } break;
      case DMG_CB_SRL_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value = DMG_ShiftRight(cpu, value);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_BIT_0_B: {
         DMG_CheckBit(cpu, 0, cpu->registers.b);
      } break;
      case DMG_CB_BIT_0_C: {
         DMG_CheckBit(cpu, 0, cpu->registers.c);
      } break;
      case DMG_CB_BIT_0_D: {
         DMG_CheckBit(cpu, 0, cpu->registers.d);
      } break;
      case DMG_CB_BIT_0_E: {
         DMG_CheckBit(cpu, 0, cpu->registers.e);
      } break;
      case DMG_CB_BIT_0_H: {
         DMG_CheckBit(cpu, 0, cpu->registers.h);
      } break;
      case DMG_CB_BIT_0_L: {
         DMG_CheckBit(cpu, 0, cpu->registers.l);
      } break;
      case DMG_CB_BIT_0_A: {
         DMG_CheckBit(cpu, 0, cpu->registers.a);
      } break;
      case DMG_CB_BIT_0_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 0, value);
      } break;

      case DMG_CB_BIT_1_B: {
         DMG_CheckBit(cpu, 1, cpu->registers.b);
      } break;
      case DMG_CB_BIT_1_C: {
         DMG_CheckBit(cpu, 1, cpu->registers.c);
      } break;
      case DMG_CB_BIT_1_D: {
         DMG_CheckBit(cpu, 1, cpu->registers.d);
      } break;
      case DMG_CB_BIT_1_E: {
         DMG_CheckBit(cpu, 1, cpu->registers.e);
      } break;
      case DMG_CB_BIT_1_H: {
         DMG_CheckBit(cpu, 1, cpu->registers.h);
      } break;
      case DMG_CB_BIT_1_L: {
         DMG_CheckBit(cpu, 1, cpu->registers.l);
      } break;
      case DMG_CB_BIT_1_A: {
         DMG_CheckBit(cpu, 1, cpu->registers.a);
      } break;
      case DMG_CB_BIT_1_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 1, value);
      } break;

      case DMG_CB_BIT_2_B: {
         DMG_CheckBit(cpu, 2, cpu->registers.b);
      } break;
      case DMG_CB_BIT_2_C: {
         DMG_CheckBit(cpu, 2, cpu->registers.c);
      } break;
      case DMG_CB_BIT_2_D: {
         DMG_CheckBit(cpu, 2, cpu->registers.d);
      } break;
      case DMG_CB_BIT_2_E: {
         DMG_CheckBit(cpu, 2, cpu->registers.e);
      } break;
      case DMG_CB_BIT_2_H: {
         DMG_CheckBit(cpu, 2, cpu->registers.h);
      } break;
      case DMG_CB_BIT_2_L: {
         DMG_CheckBit(cpu, 2, cpu->registers.l);
      } break;
      case DMG_CB_BIT_2_A: {
         DMG_CheckBit(cpu, 2, cpu->registers.a);
      } break;
      case DMG_CB_BIT_2_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 2, value);
      } break;

      case DMG_CB_BIT_3_B: {
         DMG_CheckBit(cpu, 3, cpu->registers.b);
      } break;
      case DMG_CB_BIT_3_C: {
         DMG_CheckBit(cpu, 3, cpu->registers.c);
      } break;
      case DMG_CB_BIT_3_D: {
         DMG_CheckBit(cpu, 3, cpu->registers.d);
      } break;
      case DMG_CB_BIT_3_E: {
         DMG_CheckBit(cpu, 3, cpu->registers.e);
      } break;
      case DMG_CB_BIT_3_H: {
         DMG_CheckBit(cpu, 3, cpu->registers.h);
      } break;
      case DMG_CB_BIT_3_L: {
         DMG_CheckBit(cpu, 3, cpu->registers.l);
      } break;
      case DMG_CB_BIT_3_A: {
         DMG_CheckBit(cpu, 3, cpu->registers.a);
      } break;
      case DMG_CB_BIT_3_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 3, value);
      } break;

      case DMG_CB_BIT_4_B: {
         DMG_CheckBit(cpu, 4, cpu->registers.b);
      } break;
      case DMG_CB_BIT_4_C: {
         DMG_CheckBit(cpu, 4, cpu->registers.c);
      } break;
      case DMG_CB_BIT_4_D: {
         DMG_CheckBit(cpu, 4, cpu->registers.d);
      } break;
      case DMG_CB_BIT_4_E: {
         DMG_CheckBit(cpu, 4, cpu->registers.e);
      } break;
      case DMG_CB_BIT_4_H: {
         DMG_CheckBit(cpu, 4, cpu->registers.h);
      } break;
      case DMG_CB_BIT_4_L: {
         DMG_CheckBit(cpu, 4, cpu->registers.l);
      } break;
      case DMG_CB_BIT_4_A: {
         DMG_CheckBit(cpu, 4, cpu->registers.a);
      } break;
      case DMG_CB_BIT_4_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 4, value);
      } break;

      case DMG_CB_BIT_5_B: {
      } break;
         DMG_CheckBit(cpu, 5, cpu->registers.a);
      case DMG_CB_BIT_5_C: {
         DMG_CheckBit(cpu, 5, cpu->registers.c);
      } break;
      case DMG_CB_BIT_5_D: {
         DMG_CheckBit(cpu, 5, cpu->registers.d);
      } break;
      case DMG_CB_BIT_5_E: {
         DMG_CheckBit(cpu, 5, cpu->registers.e);
      } break;
      case DMG_CB_BIT_5_H: {
         DMG_CheckBit(cpu, 5, cpu->registers.h);
      } break;
      case DMG_CB_BIT_5_L: {
         DMG_CheckBit(cpu, 5, cpu->registers.l);
      } break;
      case DMG_CB_BIT_5_A: {
         DMG_CheckBit(cpu, 5, cpu->registers.a);
      } break;
      case DMG_CB_BIT_5_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 5, value);
      } break;

      case DMG_CB_BIT_6_B: {
         DMG_CheckBit(cpu, 6, cpu->registers.b);
      } break;
      case DMG_CB_BIT_6_C: {
         DMG_CheckBit(cpu, 6, cpu->registers.c);
      } break;
      case DMG_CB_BIT_6_D: {
         DMG_CheckBit(cpu, 6, cpu->registers.e);
      } break;
      case DMG_CB_BIT_6_E: {
         DMG_CheckBit(cpu, 6, cpu->registers.d);
      } break;
      case DMG_CB_BIT_6_H: {
         DMG_CheckBit(cpu, 6, cpu->registers.h);
      } break;
      case DMG_CB_BIT_6_L: {
         DMG_CheckBit(cpu, 6, cpu->registers.l);
      } break;
      case DMG_CB_BIT_6_A: {
         DMG_CheckBit(cpu, 6, cpu->registers.a);
      } break;
      case DMG_CB_BIT_6_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 6, value);
      } break;

      case DMG_CB_BIT_7_B: {
         DMG_CheckBit(cpu, 7, cpu->registers.b);
      } break;
      case DMG_CB_BIT_7_C: {
         DMG_CheckBit(cpu, 7, cpu->registers.c);
      } break;
      case DMG_CB_BIT_7_D: {
         DMG_CheckBit(cpu, 7, cpu->registers.d);
      } break;
      case DMG_CB_BIT_7_E: {
         DMG_CheckBit(cpu, 7, cpu->registers.e);
      } break;
      case DMG_CB_BIT_7_H: {
         DMG_CheckBit(cpu, 7, cpu->registers.h);
      } break;
      case DMG_CB_BIT_7_L: {
         DMG_CheckBit(cpu, 7, cpu->registers.l);
      } break;
      case DMG_CB_BIT_7_A: {
         DMG_CheckBit(cpu, 7, cpu->registers.a);
      } break;
      case DMG_CB_BIT_7_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         DMG_CheckBit(cpu, 7, value);
      } break;

      case DMG_CB_RES_0_B: {
         cpu->registers.b &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_C: {
         cpu->registers.c &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_D: {
         cpu->registers.d &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_E: {
         cpu->registers.e &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_H: {
         cpu->registers.h &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_L: {
         cpu->registers.l &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_A: {
         cpu->registers.a &= ~(1 << 0);
      } break;
      case DMG_CB_RES_0_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 0);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_1_B: {
         cpu->registers.b &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_C: {
         cpu->registers.c &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_D: {
         cpu->registers.d &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_E: {
         cpu->registers.e &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_H: {
         cpu->registers.h &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_L: {
         cpu->registers.l &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_A: {
         cpu->registers.a &= ~(1 << 1);
      } break;
      case DMG_CB_RES_1_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 1);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_2_B: {
         cpu->registers.b &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_C: {
         cpu->registers.c &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_D: {
         cpu->registers.d &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_E: {
         cpu->registers.e &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_H: {
         cpu->registers.h &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_L: {
         cpu->registers.l &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_A: {
         cpu->registers.a &= ~(1 << 2);
      } break;
      case DMG_CB_RES_2_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 2);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_3_B: {
         cpu->registers.b &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_C: {
         cpu->registers.c &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_D: {
         cpu->registers.d &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_E: {
         cpu->registers.e &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_H: {
         cpu->registers.h &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_L: {
         cpu->registers.l &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_A: {
         cpu->registers.a &= ~(1 << 3);
      } break;
      case DMG_CB_RES_3_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 3);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_4_B: {
         cpu->registers.b &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_C: {
         cpu->registers.c &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_D: {
         cpu->registers.d &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_E: {
         cpu->registers.e &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_H: {
         cpu->registers.h &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_L: {
         cpu->registers.l &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_A: {
         cpu->registers.a &= ~(1 << 4);
      } break;
      case DMG_CB_RES_4_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 4);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_5_B: {
         cpu->registers.b &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_C: {
         cpu->registers.c &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_D: {
         cpu->registers.d &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_E: {
         cpu->registers.e &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_H: {
         cpu->registers.h &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_L: {
         cpu->registers.l &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_A: {
         cpu->registers.l &= ~(1 << 5);
      } break;
      case DMG_CB_RES_5_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 5);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_6_B: {
         cpu->registers.b &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_C: {
         cpu->registers.c &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_D: {
         cpu->registers.d &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_E: {
         cpu->registers.e &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_H: {
         cpu->registers.h &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_L: {
         cpu->registers.l &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_A: {
         cpu->registers.a &= ~(1 << 6);
      } break;
      case DMG_CB_RES_6_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 6);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_RES_7_B: {
         cpu->registers.b &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_C: {
         cpu->registers.c &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_D: {
         cpu->registers.d &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_E: {
         cpu->registers.e &= ~(1 << 7);
      case DMG_CB_RES_7_H: {
         cpu->registers.h &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_L: {
         cpu->registers.l &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_A: {
         cpu->registers.a &= ~(1 << 7);
      } break;
      case DMG_CB_RES_7_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value &= ~(1 << 7);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_0_B: {
         cpu->registers.b |= (1 << 0);
      } break;
      case DMG_CB_SET_0_C: {
         cpu->registers.c |= (1 << 0);
      } break;
      case DMG_CB_SET_0_D: {
         cpu->registers.d |= (1 << 0);
      } break;
      case DMG_CB_SET_0_E: {
         cpu->registers.e |= (1 << 0);
      } break;
      case DMG_CB_SET_0_H: {
         cpu->registers.h |= (1 << 0);
      } break;
      case DMG_CB_SET_0_L: {
         cpu->registers.l |= (1 << 0);
      } break;
      case DMG_CB_SET_0_A: {
         cpu->registers.a |= (1 << 0);
      } break;
      case DMG_CB_SET_0_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= 1 << 0;
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_1_B: {
         cpu->registers.b |= (1 << 1);
      } break;
      case DMG_CB_SET_1_C: {
         cpu->registers.c |= (1 << 1);
      } break;
      case DMG_CB_SET_1_D: {
         cpu->registers.d |= (1 << 1);
      } break;
      case DMG_CB_SET_1_E: {
         cpu->registers.e |= (1 << 1);
      } break;
      case DMG_CB_SET_1_H: {
         cpu->registers.h |= (1 << 1);
      } break;
      case DMG_CB_SET_1_L: {
         cpu->registers.l |= (1 << 1);
      } break;
      case DMG_CB_SET_1_A: {
         cpu->registers.a |= (1 << 1);
      } break;
      case DMG_CB_SET_1_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 1);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_2_B: {
         cpu->registers.b |= (1 << 2);
      } break;
      case DMG_CB_SET_2_C: {
         cpu->registers.c |= (1 << 2);
      } break;
      case DMG_CB_SET_2_D: {
         cpu->registers.d |= (1 << 2);
      } break;
      case DMG_CB_SET_2_E: {
         cpu->registers.e |= (1 << 2);
      } break;
      case DMG_CB_SET_2_H: {
         cpu->registers.h |= (1 << 2);
      } break;
      case DMG_CB_SET_2_L: {
         cpu->registers.l |= (1 << 2);
      } break;
      case DMG_CB_SET_2_A: {
         cpu->registers.a |= (1 << 2);
      } break;
      case DMG_CB_SET_2_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 2);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_3_B: {
         cpu->registers.b |= (1 << 3);
      } break;
      case DMG_CB_SET_3_C: {
         cpu->registers.c |= (1 << 3);
      } break;
      case DMG_CB_SET_3_D: {
         cpu->registers.d |= (1 << 3);
      } break;
      case DMG_CB_SET_3_E: {
         cpu->registers.e |= (1 << 3);
      } break;
      case DMG_CB_SET_3_H: {
         cpu->registers.h |= (1 << 3);
      } break;
      case DMG_CB_SET_3_L: {
         cpu->registers.l |= (1 << 3);
      } break;
      case DMG_CB_SET_3_A: {
         cpu->registers.a |= (1 << 3);
      } break;
      case DMG_CB_SET_3_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 3);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_4_B: {
         cpu->registers.b |= (1 << 4);
      } break;
      case DMG_CB_SET_4_C: {
         cpu->registers.c |= (1 << 4);
      } break;
      case DMG_CB_SET_4_D: {
         cpu->registers.d |= (1 << 4);
      } break;
      case DMG_CB_SET_4_E: {
         cpu->registers.e |= (1 << 4);
      } break;
      case DMG_CB_SET_4_H: {
         cpu->registers.h |= (1 << 4);
      } break;
      case DMG_CB_SET_4_L: {
         cpu->registers.l |= (1 << 4);
      } break;
      case DMG_CB_SET_4_A: {
         cpu->registers.a |= (1 << 4);
      } break;
      case DMG_CB_SET_4_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 4);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_5_B: {
         cpu->registers.b |= (1 << 5);
      } break;
      case DMG_CB_SET_5_C: {
         cpu->registers.c |= (1 << 5);
      } break;
      case DMG_CB_SET_5_D: {
         cpu->registers.d |= (1 << 5);
      } break;
      case DMG_CB_SET_5_E: {
         cpu->registers.e |= (1 << 5);
      } break;
      case DMG_CB_SET_5_H: {
         cpu->registers.h |= (1 << 5);
      } break;
      case DMG_CB_SET_5_L: {
         cpu->registers.l |= (1 << 5);
      } break;
      case DMG_CB_SET_5_A: {
         cpu->registers.a |= (1 << 5);
      } break;
      case DMG_CB_SET_5_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 5);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_6_B: {
         cpu->registers.b |= (1 << 6);
      } break;
      case DMG_CB_SET_6_C: {
         cpu->registers.c |= (1 << 6);
      } break;
      case DMG_CB_SET_6_D: {
         cpu->registers.d |= (1 << 6);
      } break;
      case DMG_CB_SET_6_E: {
         cpu->registers.e |= (1 << 6);
      } break;
      case DMG_CB_SET_6_H: {
         cpu->registers.h |= (1 << 6);
      } break;
      case DMG_CB_SET_6_L: {
         cpu->registers.l |= (1 << 6);
      } break;
      case DMG_CB_SET_6_A: {
         cpu->registers.a |= (1 << 6);
      } break;
      case DMG_CB_SET_6_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 6);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;

      case DMG_CB_SET_7_B: {
         cpu->registers.b |= (1 << 7);
      } break;
      case DMG_CB_SET_7_C: {
         cpu->registers.c |= (1 << 7);
      } break;
      case DMG_CB_SET_7_D: {
         cpu->registers.d |= (1 << 7);
      } break;
      case DMG_CB_SET_7_E: {
         cpu->registers.e |= (1 << 7);
      } break;
      case DMG_CB_SET_7_H: {
         cpu->registers.h |= (1 << 7);
      } break;
      case DMG_CB_SET_7_L: {
         cpu->registers.l |= (1 << 7);
      } break;
      case DMG_CB_SET_7_A: {
         cpu->registers.a |= (1 << 7);
      } break;
      case DMG_CB_SET_7_HL: {
         Uint8 value = DMG_ReadMemory(cpu, cpu->registers.hl);
         value |= (1 << 7);
         DMG_WriteMemory(cpu, cpu->registers.hl, value);
      } break;
   }
}

static inline void DMG_WriteProgramCounterToStack(DMG_CPU* cpu) {
   Uint8 program_counter_less_significant;
   Uint8 program_counter_more_significant;
   DMG_DecomposeUint16(cpu->registers.program_counter, &program_counter_less_significant, &program_counter_more_significant);

   cpu->registers.stack_pointer--;
   DMG_WriteMemory(cpu, cpu->registers.stack_pointer, program_counter_more_significant);

   cpu->registers.stack_pointer--;
   DMG_WriteMemory(cpu, cpu->registers.stack_pointer, program_counter_less_significant);
}

