#ifndef CHESS_UI_
#define CHESS_UI_

#include <SDL.h>
#include <SDL_video.h>

#define WIN_W 800
#define WIN_H 600
#define IMG_W 60
#define IMG_H 74
#define BUTTON_H 70
#define BUTTON_W 200

#define WIN_HEIGHT 600
#define WIN_WIDTH 800
#define BUTTON 'b'
#define PANEL 'p'
#define WINDOW 'w'
#define LABEL 'l'



//struct UINode* mainWindow;
//struct UINode* settingWindow;
//struct UINode* playerSelectionWindow;
//struct UINode* boardSettingsWindow;
//struct UINode* gameWindow;

char lastChosenTool;

int shouldQuitMainEvents;
int shouldQuitBoardSeEvents;
int shouldQuitGameEvents;
int shouldQuitsettingEvents;
int shouldQuitSelectionEvents;
//Pos posToMoveFrom;

//structures
typedef struct button
{
	SDL_Surface * surface;
	//char type;
	char* name;
	SDL_Rect* rect;
	char * filename;
	int x;
	int y;
} Button;

typedef struct label
{
	SDL_Surface * surface;
	//char type;
	char* name;
	SDL_Rect* rect;
	char * filename;
	int x;
	int y;
} Label;

//Panel contains multiple surfaces (up to 20)
typedef struct panel
{
	SDL_Surface * surface;
	//char type;
	char* name;
	SDL_Rect* rect;
	int x;
	int y;
	int width;
	int height;
} Panel;

typedef struct window
{
	SDL_Surface * surface;
	//char type;
	char* title;
	int width;
	int height;
} Window;

typedef struct treenode
{
	void* control;
	char type;//type of the control - button, panel, lable,window
	void(*Action)(void*);//this function will be the logic for the specific control
	int child_num;
	struct treenode** children;
	struct treenode* parent;
} TreeNode;


#endif CHESS_UI_