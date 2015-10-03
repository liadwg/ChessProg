#include "chess_ui.h"

#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

int glob_quit = 0;
TreeNode *mainMenu;
TreeNode *settingsMenu;
TreeNode *playerSelection;
TreeNode *boardSettings;
TreeNode *gameWindow;

TreeNode* new_node(void* control, int child_num, TreeNode** children, TreeNode* parent){
	TreeNode *node = malloc(sizeof(TreeNode));
	node->child_num = child_num;
	node->children = children;
	node->parent = parent;
	node->control = control;
	return node;
}

TreeNode* new_window(char *title, int width, int height, int children){
	SDL_WM_SetCaption(title, title);
	SDL_Surface *w = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	Window *res = malloc(sizeof(Window));
	res->title = title;
	res->width = width;
	res->height = height;
	res->surface = w;
	return new_node(res, children, NULL, NULL);
}
//
//TreeNode* new_panel(char* name, int x, int y, int width, int height, int children, TreeNode *parent){
//	SDL_Surface *w = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
//	if (w == NULL) {
//		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
//		return 1;
//	}
//	SDL_Rect parent_rect = { x, y, 0, 0 };
//	SDL_Rect inner_rect = { x, y, width, height };
//	SDL_BlitSurface((Window) parent->control.surface, )
//	Panel *res = malloc(sizeof(Panel));
//	res->name = name;
//	res->width = width;
//	res->height = height;
//	res->surface = w;
//	return new_node(res, children, NULL, NULL);
//}

int init_main_menu(){
	TreeNode *window = new_window("Chess Main Menu", WIN_W / 2, (WIN_H * 2) / 3, 1);

}

int game_screen(){}

int settings_screen(){}

int main_menu(SDL_Surface *w){
	SDL_Event e;
	SDL_Rect buttons[3] = { { WIN_W / 2 - BUTTON_W / 2, 110, BUTTON_W, BUTTON_H },
							{ WIN_W / 2 - BUTTON_W / 2, 120 + BUTTON_H, BUTTON_W, BUTTON_H },
							{ WIN_W / 2 - BUTTON_W / 2, 130 + BUTTON_H * 2, BUTTON_W, BUTTON_H } };

	if (SDL_FillRect(w, 0, 0) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return 1;
	}

	foreach(SDL_Rect *button, buttons){
		if (SDL_FillRect(w, button, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			return 1;
		}
	}

	/* We finished drawing*/
	if (SDL_Flip(w) != 0) {
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		return 1;
	}

	/* Poll for keyboard & mouse events*/
	while (!glob_quit) {
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case (SDL_QUIT) :
				glob_quit = 1;
				break;
			case (SDL_KEYUP) :
				if (e.key.keysym.sym == SDLK_ESCAPE) glob_quit = 1;
				break;
			case (SDL_MOUSEBUTTONUP) :
				if ((e.button.x > buttons[2].x) && (e.button.x < buttons[2].x + buttons[2].w) && (e.button.y > buttons[2].y) && (e.button.y < buttons[2].y + buttons[2].h))
					glob_quit = 1;
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

int test_animate(SDL_Surface *w){
	SDL_Event e;
	SDL_Rect rect = { 10, 10, 50, 50 };
	SDL_Rect imgrect = { 0, 0, IMG_W, IMG_H };
	SDL_Surface *img = SDL_LoadBMP("/Users/liadwg/Documents/visual studio 2013/Projects/ChessProg/test2.bmp");
	int quit = 0;

	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	//atexit(SDL_Quit);

	/* Define the rects we need*/


	/* Load test spritesheet image*/

	if (img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}

	/* Set colorkey to BLUE*/
	if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 255)) != 0) {
		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
		SDL_FreeSurface(img);
		return 1;
	}


	while (!quit) {
		/* Clear window to BLACK*/
		if (SDL_FillRect(w, 0, 0) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Green rectangle button*/
		if (SDL_FillRect(w, &rect, SDL_MapRGB(w->format, 0, 255, 0)) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Draw image sprite*/
		if (SDL_BlitSurface(img, &imgrect, w, 0) != 0) {
			SDL_FreeSurface(img);
			printf("ERROR: failed to blit image: %s\n", SDL_GetError());
			break;
		}

		/* Advance to next sprite*/
		imgrect.x += imgrect.w;
		if (imgrect.x >= img->w) {
			imgrect.x = 0;
			imgrect.y += imgrect.h;
			if (imgrect.y >= img->h) imgrect.y = 0;
		}

		/* We finished drawing*/
		if (SDL_Flip(w) != 0) {
			printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
			break;
		}

		/* Poll for keyboard & mouse events*/

		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case (SDL_QUIT) :
				quit = 1;
				break;
			case (SDL_KEYUP) :
				if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				break;
			case (SDL_MOUSEBUTTONUP) :
				if ((e.button.x > rect.x) && (e.button.x < rect.x + rect.w) && (e.button.y > rect.y) && (e.button.y < rect.y + rect.h))
					quit = 1;
				break;
			default:
				break;
			}
		}

		/* Wait a little before redrawing*/
		SDL_Delay(10);
	}

	SDL_FreeSurface(img);
	atexit(SDL_Quit);
	return 0;
}

int main_sdl(void) {
	SDL_Event e;
	SDL_Surface *w = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	int quit = 0;

	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	//atexit(SDL_Quit);

	/* Create window surface*/

	if (w == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	
	//test_animate(w);
	main_menu(w);
	atexit(SDL_Quit);
	return 0;
}