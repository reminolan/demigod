#include "demigod.h"

static struct {
   SDL_Window* window;
   SDL_Renderer* renderer;
} dmg_display;

bool DMG_InitDisplay() {
   dmg_display.window = SDL_CreateWindow("demigod", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
   if (!dmg_display.window) {
      //TODO: logging
      return false;
   }

   dmg_display.renderer = SDL_CreateRenderer(dmg_display.window, NULL);
   if (!dmg_display.renderer) {
      //TODO: logging
      return false;
   }

   SDL_ShowWindow(dmg_display.window);
   return true;
}

void DMG_QuitDisplay() {
   SDL_HideWindow(dmg_display.window);

   if (dmg_display.renderer) {
      SDL_DestroyRenderer(dmg_display.renderer);
   }

   if (dmg_display.window) {
      SDL_DestroyWindow(dmg_display.window);
   }
}

void DMG_TickDisplay() {
   SDL_RenderPresent(dmg_display.renderer);

   SDL_SetRenderDrawColor(dmg_display.renderer, 0, 0, 0, 255);
   SDL_RenderClear(dmg_display.renderer);
}

void DMG_ToggleFullscreen() {
   SDL_WindowFlags flags = SDL_GetWindowFlags(dmg_display.window);
   SDL_SetWindowFullscreen(dmg_display.window, !(flags & SDL_WINDOW_FULLSCREEN));
}

SDL_Window* DMG_GetWindow() {
   return dmg_display.window;
}

SDL_Renderer* DMG_GetRenderer() {
   return dmg_display.renderer;
}

