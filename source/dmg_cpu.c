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

static inline Uint8 DMG_AddNibble(Uint8 augend, Uint8 addend, Uint8 in_carry, Uint8* out_carry);

static inline Uint8 DMG_Add8(DMG_CPU* cpu, Uint8 augend, Uint8 addend);

static inline Uint8 DMG_AddWithCarry8(DMG_CPU* cpu, Uint8 augend, Uint8 addend);

static inline Uint16 DMG_Add16(DMG_CPU* cpu, Uint16 augend, Uint16 addend);

static inline Uint8 DMG_Subtract8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend);

static inline Uint8 DMG_SubtractWithCarry8(DMG_CPU* cpu, Uint8 minuend, Uint8 subtrahend);

static inline Uint8 DMG_And(DMG_CPU* cpu, Uint8 left, Uint8 right);

static inline Uint8 DMG_Xor(DMG_CPU* cpu, Uint8 left, Uint8 right);

static inline Uint8 DMG_Or(DMG_CPU* cpu, Uint8 left, Uint8 right);

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
      case DMG_OP_JR_Z_e:
         Sint8 offset = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            cpu->registers.program_counter += offset;
         }
         break;
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
         Uint8 nn_less_signficant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         DMG_WriteProgramCounterToStack(cpu);
         cpu->registers.program_counter = DMG_ComposeUint16(nn_less_signficant, nn_more_significant);
      } break;
      case DMG_OP_CALL_Z_nn: {
         Uint8 nn_less_signficant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (cpu->registers.flags & DMG_CPU_FLAG_ZERO) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_signficant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_C_nn: {
         Uint8 nn_less_signficant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (cpu->registers.flags & DMG_CPU_FLAG_CARRY) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_signficant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_NZ_nn: {
         Uint8 nn_less_signficant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (!(cpu->registers.flags & DMG_CPU_FLAG_ZERO)) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_signficant, nn_more_significant);
         }
      } break;
      case DMG_OP_CALL_NC_nn: {
         Uint8 nn_less_signficant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);

         if (!(cpu->registers.flags & DMG_CPU_FLAG_CARRY)) {
            DMG_WriteProgramCounterToStack(cpu);
            cpu->registers.program_counter = DMG_ComposeUint16(nn_less_signficant, nn_more_significant);
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
         offset += (Uint8)(((Uint16)cpu->registers.a + offset) % 255);
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
         cpu->registers.bc = DMG_ComposeUint16(nn_less_significant, nn_more_signifcant);
      } break;
      case DMG_OP_LD_DE_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.de = DMG_ComposeUint16(nn_less_significant, nn_more_signifcant);
      } break;
      case DMG_OP_LD_HL_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.hl = DMG_ComposeUint16(nn_less_significant, nn_more_signifcant);
      } break;
      case DMG_OP_LD_SP_nn: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         cpu->registers.sp = DMG_ComposeUint16(nn_less_significant, nn_more_signifcant);
      } break;
      case DMG_OP_LD_n16_SP: {
         Uint8 nn_less_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint8 nn_more_significant = DMG_ReadMemory(cpu, cpu->registers.program_counter++);
         Uint16 nn = DMG_ComposeUint16(nn_less_significant, nn_more_significant);

         Uint8 sp_less_significant;
         Uint8 sp_more_signficaint;
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
      } break;
      case DMG_OP_INC_BC: {
      } break;
      case DMG_OP_DEC_BC: {
      } break;
      case DMG_OP_INC_DE: {
      } break;
      case DMG_OP_DEC_DE: {
      } break;
      case DMG_OP_INC_HL: {
      } break;
      case DMG_OP_DEC_HL: {
      } break;
      case DMG_OP_INC_SP: {
      } break;
      case DMG_OP_DEC_SP: {
      } break;
      case DMG_OP_ADD_HL_BC: {
      } break;
      case DMG_OP_ADD_HL_DE: {
      } break;
      case DMG_OP_ADD_HL_HL: {
      } break;
      case DMG_OP_ADD_HL_SP: {
      } break;
      case DMG_OP_ADD_SP_e: {
      } break;
      case DMG_OP_RLCA: {
      } break;
      case DMG_OP_RRCA: {
      } break;
      case DMG_OP_RLA: {
      } break;
      case DMG_OP_RRA: {
      } break;
      case DMG_OP_CB_op: {
      } break;
      case DMG_OP_NOP: {
      } break;
      case DMG_OP_STOP: {
      } break;
      case DMG_OP_HALT: {
      } break;
      case DMG_OP_DI: {
      } break;
      case DMG_OP_EI: {
      } break;
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

static inline Uint8 DMG_AddNibble(Uint8 augend, Uint8 addend, Uint8 in_carry, Uint8* out_carry) {
   Uint8 result = (augend & 0xF) + (addend & 0xF) + (in_carry & 0b1);

   if (out_carry) {
      *out_carry = (result >> 4) & 1;
   }

   return result;
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

static inline void DMG_WriteProgramCounterToStack(DMG_CPU* cpu) {
   Uint8 program_counter_less_significant;
   Uint8 program_counter_more_significant;
   DMG_DecomposeUint16(cpu->registers.program_counter, &program_counter_less_significant, &program_counter_more_significant);

   cpu->registers.stack_pointer--;
   DMG_WriteMemory(cpu, cpu->registers.stack_pointer, program_counter_more_significant);

   cpu->registers.stack_pointer--;
   DMG_WriteMemory(cpu, cpu->registers.stack_pointer, program_counter_less_significant);
}

