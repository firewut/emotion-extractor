#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "classes/world.h"
#include "classes/helpers.h"

#define DEFAULT_WIDTH  640
#define DEFAULT_HEIGHT 480
#define WINDOW_TITLE   "TEST RPG"

int main() {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *surface;
	SDL_Texture *texture;
	SDL_Event event;


	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Couldn't initialize SDL: %s",
				SDL_GetError()
			);
			return 1;
	}

	window = SDL_CreateWindow(
    WINDOW_TITLE,                     // window title
    SDL_WINDOWPOS_UNDEFINED,           // initial x position
    SDL_WINDOWPOS_UNDEFINED,           // initial y position
    DEFAULT_WIDTH,                     // width, in pixels
    DEFAULT_HEIGHT,                    // height, in pixels
    SDL_WINDOW_RESIZABLE               // flags - see below
  );

	if (window == NULL) {
		SDL_LogError(
			SDL_LOG_CATEGORY_APPLICATION,
			"Couldn't create window: %s",
			SDL_GetError()
		);
		return 1;
	}

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_PRESENTVSYNC
	);
	if (renderer == NULL){
		SDL_LogError(
			SDL_LOG_CATEGORY_APPLICATION,
			"Couldn't create renderer: %s",
			SDL_GetError()
		);
		return 1;
	}

	surface = SDL_CreateRGBSurface(0, 50, 50, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 66, 66, 66));
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Create new World
	World *world = NewWorld(
		randstring(10),
		randstring(5)
	);
	WorldAddRandomCharacter(world);

	while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        break;
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

		SDL_Delay(1000/60);
  }

	DestroyWorld(world);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
