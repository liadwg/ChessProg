#ifndef CHESS_UI_
#define CHESS_UI_

#include <SDL.h>
#include <SDL_video.h>

#ifndef CHESS_UTILS_
#include "chess_utils.h"
#endif

#ifndef CHESS_LOGICS_
#include "chess_logics.h"
#endif

#define WIN_W 800
#define WIN_H 600
#define IMG_W 60
#define IMG_H 74
#define BUTTON_H 50
#define BUTTON_W 100
#define TILE 65
#define SAVE_SLOTS 7 // remove after added to chessprog.h

#define WIN_HEIGHT 600
#define WIN_WIDTH 800
#define BOARD_SIZE 8

#define malloc(x) safe_malloc(x)
#define realloc(x, y) safe_realloc((x), (y))
#define fgetc(x) safe_fgetc(x)
#define free(x) safe_free(x)

#define printf(...) \
	if (printf(__VA_ARGS__) < 0){ \
		perror_message("printf"); \
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]); \
		abort();} \
				else (void)0

//structures
typedef struct button
{
	char* name;
	SDL_Surface * img;
	int x;
	int y;
	int width;
	int height;
	int abs_x; // to handle clicks more efficiantly
	int abs_y; // to handle clicks more efficiantly
	void(*handler)(int); // pointer to the button handler function
	int args; // handler arg
} Button;

typedef struct label
{
	char* name;
	SDL_Surface * img;
	int x;
	int y;
	int width;
	int height;
} Label;

typedef struct panel
{
	char* name;
	SDL_Surface * img;
	int x;
	int y;
	int width;
	int height;
} Panel;

typedef struct window
{
	SDL_Surface * surface;
	char* title;
	int width;
	int height;
} Window;

typedef enum { WINDOW = 0, PANEL = 1, LABEL = 2, BUTTON = 3 } CONTROL;

typedef struct treenode
{
	void* control;
	CONTROL type;
	int child_num;
	struct treenode** children;
	struct treenode* parent;
} TreeNode;


// Globals
// Framework Globals
extern Button **buttons;
extern int buttons_count;

// Screens Globals
extern TreeNode *mainMenu;
extern TreeNode *loadSave;
extern TreeNode *AIsettingsMenu;
extern TreeNode *playerSelection;
extern TreeNode *boardSetting;
extern TreeNode *gameWindow;
extern TreeNode *prevScreen;
extern TreeNode *currScreen;

// Game-Flow Globals
extern int glob_quit;
extern int start_game;
extern int board_ready;
extern int back2settings;

// Game Globals
extern char gui_board[BOARD_SIZE][BOARD_SIZE];
extern char tmp_board[BOARD_SIZE][BOARD_SIZE];
extern char piece_picked;
extern Pos *src_pos;
extern Move *move_to_do;
extern int wait4promote;
extern TreeNode *tmp_panel;

// GUI Framework funcs
TreeNode* new_node(void* control, CONTROL type, int child_num, TreeNode* parent);
TreeNode* new_window(char *title, int width, int height, int children);
TreeNode* new_panel(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file);
TreeNode* new_label(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file);
TreeNode* new_button(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file, void(*handler)(int), int args);
void add_label_to_button(TreeNode *button, char* pic);
void remove_label_from_button(TreeNode *button);
int draw_tree_rec(Window* root, TreeNode* node);
int draw_tree(TreeNode* root);
void free_tree(TreeNode *node);
void get_screen_buttons(TreeNode *node);
int is_click_on_button(int x, int y, Button *button);
void run_events_loop(TreeNode* screen);


// Chess UI funcs
void quit_all();
char* get_piece_pic(char piece);
char get_piece_by_pic(char* pic);
void alert_state(int state, COLOR player);
TreeNode* get_button_node(TreeNode *node, int arg);
void update_board_gui(TreeNode *board_node, char board[BOARD_SIZE][BOARD_SIZE]);
void show_best_move(int depth);
void set_piece_picked(int i);
Move* generate_move(int col, int row);
void tile_clicked(int tile);
void screen_dismissed(TreeNode *screen);
void cancel_clicked();
void set_depth(int i);
void set_player(int i);
void set_next(COLOR i);
void set_player_color(COLOR i);
void board_set_ok();
void load_slot(int slot);
void save_slot(int slot);
void open_board_setting();
void start_game_clicked();
void open_load_save(int i);
void open_player_selection();
void open_main_menu();
void init_promote_view();
void init_game_window();
void init_board_setting();
void init_player_selection();
void init_AI_setting();
void init_load_save(int load_save);
void init_main_menu();
Move* gui_game_mode(char board[BOARD_SIZE][BOARD_SIZE]);
int gui_setting_mode();

#endif