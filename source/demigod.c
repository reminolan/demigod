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

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void** sdl_app_state, int arguments_count, char** arguments_array) {
   if (!SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_GAMEPAD))
      return SDL_APP_FAILURE;

   if (!DMG_InitDisplay())
      return false;

   return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* sdl_app_state, SDL_Event* event) {
   if (event->type == SDL_EVENT_QUIT)
      return SDL_APP_SUCCESS;

   if (event->type == SDL_EVENT_KEY_UP && event->key.key == SDLK_F4)
      DMG_ToggleFullscreen();

   return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* sdl_app_state) {
   DMG_TickDisplay();

   return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* sdl_app_state, SDL_AppResult result) {
   DMG_QuitDisplay();
}

