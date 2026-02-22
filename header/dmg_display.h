#ifndef DMG_DISPLAY_H
#define DMG_DISPLAY_H

#include <SDL3/SDL.h>

bool DMG_InitDisplay();

void DMG_QuitDisplay();

void DMG_TickDisplay();

void DMG_ToggleFullscreen();

SDL_Window* DMG_GetWindow();

SDL_Renderer* DMG_GetRenderer();

#endif//DMG_DISPLAY_H
