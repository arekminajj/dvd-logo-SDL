#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 1000
#define LOGO_HEIGHT 154
#define LOGO_WIDTH 256

SDL_Window* win;
SDL_Surface* winSurface;
SDL_Surface* images[4];
TTF_Font* font;
SDL_Surface* textSurface;
SDL_Color textColor = {255, 255, 255};
char* text;

bool loadLogos()
{
	SDL_Surface* temps[4];

	temps[0] = IMG_Load("assets/DVD_logo_blue.svg");
	temps[1] = IMG_Load("assets/DVD_logo_pink.svg");
	temps[2] = IMG_Load("assets/DVD_logo_white.svg");
	temps[3] = IMG_Load("assets/DVD_logo_yellow.svg");
	
	for(int i = 0; i < 4; i++)
	{
		if(temps[i] == NULL)
		{
			printf("Error loading logo!\n");
			return false;
		}
	}

	for(int i = 0; i<4; i++)
	{
		images[i] = SDL_ConvertSurface(temps[i], winSurface->format, 0);
		SDL_FreeSurface(temps[i]);
		if(images[i] == NULL)
		{
			printf("Error converting surface!\n");
			return false;
		}
	}
	return true;
}

int main()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("Error initializing SDL2!\n");
		return -1;
	}

	win = SDL_CreateWindow("dvd-logo", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if(win == NULL)
	{
		printf("Error creating a window!\n");
	}

	winSurface = SDL_GetWindowSurface(win);

	if(!loadLogos()) 
	{
		return -1;
	}

	if(TTF_Init() < 0)
	{
		printf("Error initializing TTF!\n");
		return -1;
	}

	font = TTF_OpenFont("assets/roboto.ttf", 24);
	if(font == NULL)
	{
		printf("Error loading the font!\n");
		return -1;
	}

	//30 should be ok.
	text = (char*)malloc(30);

	SDL_Event ev;
	bool running = true;

	SDL_Rect dest;
	dest.x = 0;
	dest.y = 0;
	dest.w = LOGO_WIDTH;
	dest.h = LOGO_HEIGHT;

	int xspeed = 2;
	int yspeed = 2;
	int imageIndex = 0;

	static int cornersCount = 0;
	Uint32 lastUpdate = SDL_GetTicks();

	while ( running ) {	
		Uint64 start = SDL_GetPerformanceCounter();
		while ( SDL_PollEvent( &ev ) != 0 ) {
			switch (ev.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}	

		dest.x += xspeed;
		dest.y += yspeed;

		//CHECK FOR CORNERS
		if(dest.x >= WINDOW_WIDTH - LOGO_WIDTH && dest.y >= WINDOW_HEIGHT - LOGO_HEIGHT)
			cornersCount++;
		if(dest.x <= 0 && dest.y <= 0)
			cornersCount++;
		if(dest.x <= 0 && dest.y >= WINDOW_HEIGHT - LOGO_HEIGHT)
			cornersCount++;
		if(dest.y <= 0 && dest.x >= WINDOW_WIDTH - LOGO_WIDTH)
			cornersCount++;

		if(dest.x >= WINDOW_WIDTH - LOGO_WIDTH || dest.x <= 0)
		{
			xspeed *= -1;
			imageIndex = (imageIndex + 1) % 4;
		}

		if(dest.y >= WINDOW_HEIGHT - LOGO_HEIGHT || dest.y <= 0)
		{
			yspeed *= -1;
			imageIndex = (imageIndex + 1) % 4;
		}

		SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 0, 0, 0));
		SDL_BlitScaled(images[imageIndex], NULL, winSurface, &dest);

		if (textSurface) SDL_FreeSurface(textSurface);

		sprintf(text, "Corners: %d", cornersCount);
		textSurface = TTF_RenderText_Solid(font, text, textColor);
		SDL_Rect textDest;
		textDest.x = WINDOW_WIDTH / 2 - 60;
		textDest.y = 0;
		SDL_BlitSurface(textSurface, NULL, winSurface, &textDest);

		SDL_UpdateWindowSurface(win);
		
		Uint64 end = SDL_GetPerformanceCounter();
		float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		SDL_Delay(floor(16.666f - elapsedMS));
	}

	//delete
	for(int i = 0; i < 4; i++) { SDL_FreeSurface(images[i]); }
	SDL_FreeSurface(winSurface);
	SDL_FreeSurface(textSurface);
	SDL_DestroyWindow(win);
	win = NULL;
	winSurface = NULL;
	TTF_CloseFont(font);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	free(text);

	return 0;
}
