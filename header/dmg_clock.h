#ifndef DMG_CLOCK_H
#define DMG_CLOCK_H

typedef void DMG_ClockHandle;

DMG_ClockHandle* DMG_InitClock();

void DMG_QuitClock(DMG_ClockHandle* clock_handle);

#endif//DMG_CLOCK_H
