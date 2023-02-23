// 2sdl.h
// outputs current Bear API context to an SDL window
// March 16 2018
// Gabriel Campbell
//

#ifndef _2SDL_H
#define _2SDL_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#undef main

#include "br.h"

void sdl_init();	// initialize SDL
SDL_Window* sdl_create_window(const char* title, uint32_t pixel_size);	// create an SDL window
SDL_Renderer* sdl_create_renderer(SDL_Window* host);	// create an SDL renderer
bool sdl_draw(SDL_Renderer* renderer, uint32_t pixel_size);	// draw to and present renderer. Returns false on error.

void sdl_init()
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		exit(-1);
	atexit(SDL_Quit);
}

SDL_Window* sdl_create_window(const char* title, uint32_t pixel_size)
{
	if(!_brcontext)
		return NULL;	// no bound context
	
	SDL_Window* win = NULL;
	win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		_brcontext->rb_width * pixel_size, _brcontext->rb_height * pixel_size, SDL_WINDOW_SHOWN);
	if(!win)
		exit(-1);
	return win;
}

SDL_Renderer* sdl_create_renderer(SDL_Window* host)
{
	SDL_Renderer* ren = NULL;
	ren = SDL_CreateRenderer(host, -1, SDL_RENDERER_ACCELERATED);
	
	if(!ren)
		exit(-1);
	return ren;
}

bool sdl_draw(SDL_Renderer* renderer, uint32_t pixel_size)
{
	if(!_brcontext)
		return false;	// no bound context
	
	int render_width = 0;
	int render_height = 0;
	
	int result = SDL_GetRendererOutputSize(renderer, &render_width, &render_height);
	if(result != 0)
	{
		printf("sdl_draw: couldn't get renderer dimensions\n");
		return false;	// could not get renderer dimensions
	}
		
	int render_pixel_width = render_width / pixel_size;
	int render_pixel_height = render_height / pixel_size;
	
	if(_brcontext->rb_width != render_pixel_width || _brcontext->rb_height != render_pixel_height)
	{
		printf("sdl_draw: incompatible buffer dimensions\n");
		return false;		// buffer incompatible with renderer
	}
		
	SDL_Texture* target = NULL;
	target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		render_pixel_width, render_pixel_height);
	
	if(!target)
	{
		printf("sdl_draw: failed to create target texture\n");
		return false;	// failed to create target texture
	}
		
	Uint32* pixels = NULL;
	int pitch = render_pixel_width * 4;
	
	// lock target
	if(SDL_LockTexture(target, NULL, (void**)&pixels, &pitch) != 0)
	{
		printf("sdl_draw: failed to lock target texture\n");
		return false;
	}
		
	// map pixels from buffer to target
	for(int y = 0; y < render_pixel_height; y += 1)
		for(int x = 0; x < render_pixel_width; x += 1)
		{
			uint32_t index = x + y * render_pixel_width;
			uint32_t col = 0;
			
			uint32_t r = 0;
			uint32_t g = 0;
			uint32_t b = 0;
			
			Uint32* p = 
				&pixels[index];
				
			switch(_brcontext->cb_type)
			{
				case BR_R8G8B8A8:
					col = ( (uint32_t*)_brcontext->cb ) [index];
					r = _BR_R8G8B8A8_R(col);
					g = _BR_R8G8B8A8_G(col);
					b = _BR_R8G8B8A8_B(col);
					*p = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888),
						r, g, b);
					break;
				case BR_R5G5B5A1:
					col = ( (uint16_t*)_brcontext->cb ) [index];
					r = _BR_R5G5B5A1_R(col)*8.22580645161f;
					g = _BR_R5G5B5A1_G(col)*8.22580645161f;
					b = _BR_R5G5B5A1_B(col)*8.22580645161f;
					*p = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888),
						r, g, b);
					break;
				default:
					printf("2sdl error: color buffer not R8G8B8A8\n");
					return false;
			}
		}
		
	SDL_UnlockTexture(target);
	
	SDL_Rect dst;
	dst.x = 0, dst.y = 0;
	dst.w = render_width;
	dst.h = render_height;
	
	SDL_RenderCopy(renderer, target, NULL, &dst);
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	return true;
}
#endif
