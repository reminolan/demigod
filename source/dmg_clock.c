#include "demigod.h"

#include <SDL3/SDL.h>
#include <stdio.h>

typedef struct DMG_Clock {
   bool valid;
   SDL_Thread* thread;
} DMG_Clock;

static int SDLCALL ClockThreadFunction(void*  pointer) {
   Uint64 start_time = SDL_GetPerformanceCounter();
   DMG_Clock* clock = (DMG_Clock*)pointer;

   while (clock->valid) {
      Uint64 end_time = SDL_GetPerformanceCounter();
      double seconds = (double)(end_time - start_time) / (double)SDL_GetPerformanceFrequency();
      start_time = end_time;

      double nanoseconds = seconds * 1'000'000'000;

      printf("took nanoseconds %f\n", nanoseconds);

      if (nanoseconds < 1000) {
         Uint64 delay = 1000 - (Uint64)nanoseconds;
         SDL_DelayPrecise(delay);
         printf("delay %lu\n", delay);
      }
   }

   return 0;
}

DMG_ClockHandle* DMG_InitClock() {
   DMG_Clock* clock = (DMG_Clock*)SDL_malloc(sizeof(DMG_Clock));

   if (clock) {
      clock->thread = SDL_CreateThread(ClockThreadFunction, "Demigod-EmulationThread", clock);
      clock->valid = true;
   }

   return (DMG_ClockHandle*)clock;
}

void DMG_QuitClock(DMG_ClockHandle* handle) {
   if (handle) {
      DMG_Clock* clock = (DMG_Clock*)handle;
      clock->valid = false;

      int result;
      SDL_WaitThread(clock->thread, &result);

      clock->thread = nullptr;
   }
}


