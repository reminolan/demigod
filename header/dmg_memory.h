#ifndef DMG_MEMORY_H
#define DMG_MEMORY_H

struct DMG_CPU;

Uint8 DMG_ReadMemory(struct DMG_CPU* cpu, Uint16 address);

void DMG_WriteMemory(struct DMG_CPU* cpu, Uint16 address, Uint8 value);

#endif//DMG_MEMORY_H
