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

