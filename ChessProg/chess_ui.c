#include "chess_ui.h"

#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

int glob_quit = 0;
int board_ready = 0;
char piece_picked = NULL;
TreeNode *mainMenu = NULL;
TreeNode *loadSave = NULL;
TreeNode *settingsMenu = NULL;
TreeNode *playerSelection = NULL;
TreeNode *boardSetting = NULL;
TreeNode *gameWindow = NULL;
TreeNode *prevScreen = NULL;
TreeNode *currScreen = NULL;
char curr_board[BOARD_SIZE][BOARD_SIZE];
char tmp_board[BOARD_SIZE][BOARD_SIZE];


void quit_all(){
	glob_quit = 1;
	if (mainMenu != NULL) free_tree(mainMenu);
	if (settingsMenu != NULL) free_tree(settingsMenu);
	if (playerSelection != NULL) free_tree(playerSelection);
	if (boardSetting != NULL) free_tree(boardSetting);
	if (gameWindow != NULL) free_tree(gameWindow);
	atexit(SDL_Quit);
	return 0;
}

TreeNode* get_button_node(TreeNode *node, int arg){
	if (node->type == BUTTON){
		Button *btn = (Button*)node->control;
		if (btn->args == arg) return node;
	}
	for (int i = 0; i < node->child_num; i++)
		if (get_button_node(node->children[i], arg) != NULL) 
			return get_button_node(node->children[i], arg);
	return NULL;
}

char* get_piece_pic(char piece){
	switch (piece){
	case WHITE_P: return "pics/pawn_w.bmp";
	case BLACK_P: return "pics/pawn_b.bmp";
	case WHITE_B: return "pics/bishop_w.bmp"; 
	case BLACK_B: return "pics/bishop_b.bmp";
	case WHITE_R: return "pics/rook_w.bmp"; 
	case BLACK_R: return "pics/rook_b.bmp";
	case WHITE_N: return "pics/knight_w.bmp";
	case BLACK_N: return "pics/knight_b.bmp";
	case WHITE_Q: return "pics/queen_w.bmp"; 
	case BLACK_Q: return "pics/queen_b.bmp";
	case WHITE_K: return "pics/king_w.bmp"; 
	case BLACK_K: return "pics/king_b.bmp";
	}
}

char get_piece_by_pic(char* pic){
	if (strcmp(pic, get_piece_pic(WHITE_P)) == 0) return WHITE_P;
	if (strcmp(pic, get_piece_pic(BLACK_P)) == 0) return BLACK_P;
	if (strcmp(pic, get_piece_pic(WHITE_B)) == 0) return WHITE_B;
	if (strcmp(pic, get_piece_pic(BLACK_B)) == 0) return BLACK_B;
	if (strcmp(pic, get_piece_pic(WHITE_R)) == 0) return WHITE_R;
	if (strcmp(pic, get_piece_pic(BLACK_R)) == 0) return BLACK_R;
	if (strcmp(pic, get_piece_pic(WHITE_N)) == 0) return WHITE_N;
	if (strcmp(pic, get_piece_pic(BLACK_N)) == 0) return BLACK_N;
	if (strcmp(pic, get_piece_pic(WHITE_Q)) == 0) return WHITE_Q;
	if (strcmp(pic, get_piece_pic(BLACK_Q)) == 0) return BLACK_Q;
	if (strcmp(pic, get_piece_pic(WHITE_K)) == 0) return WHITE_K;
	if (strcmp(pic, get_piece_pic(BLACK_K)) == 0) return BLACK_K;
}

void update_board_gui(TreeNode *board_node, char board[BOARD_SIZE][BOARD_SIZE]){
	TreeNode *btn_node;
	Button *button;
	for (int i = 0; i < board_node->child_num; i++){
		btn_node = board_node->children[i];
		button = (Button*)btn_node->control;
		if (btn_node->child_num != 0) remove_label_from_button(btn_node);
		if (board[button->args / 10][button->args % 10] != EMPTY) add_label_to_button(btn_node, get_piece_pic(board[button->args / 10][button->args % 10]));
	}
}

void set_piece_picked(char piece){
	piece_picked = piece;
}

void tile_clicked(int tile){
	TreeNode *tile_node = get_button_node(currScreen, tile);
	Button *button = (Button*)tile_node->control;
	if (currScreen == boardSetting){
		if (piece_picked == NULL && tile_node->child_num == 0) return;
		else if (piece_picked != NULL){
			tmp_board[button->args / 10][button->args % 10] = piece_picked;
			//add_label_to_button(tile_node, get_piece_pic(piece_picked));
			piece_picked = NULL;
		}
		else{ // if (piece_picked == NULL && tile_node->child_num != 0)
			Label *lbl = (Label*)tile_node->children[0]->control;
			piece_picked = get_piece_by_pic(lbl->name);
			tmp_board[button->args / 10][button->args % 10] = EMPTY;
		}
		update_board_gui(tile_node->parent, tmp_board);
		draw_tree(currScreen);
	} 
}

void board_set_ok(){
	duplicate_board(tmp_board, curr_board);
	//start_game();
}

void set_player(int i){
	game_mode = i;
}

void set_next(COLOR i){
	//start_color = i;
}

void screen_dismissed(TreeNode *screen){
	if (screen == mainMenu) mainMenu = NULL;
	else if (screen == settingsMenu) settingsMenu = NULL;
	else if (screen == playerSelection) playerSelection = NULL;
	else if (screen == boardSetting) boardSetting = NULL;
	else if (screen == gameWindow) gameWindow = NULL;
}

void cancel_clicked(){
	if (currScreen == prevScreen) prevScreen = mainMenu; // make sure its ok in all flows
	free_tree(currScreen);
	screen_dismissed(currScreen);
	draw_tree(prevScreen);
	currScreen = prevScreen;
}

void init_board_setting(){
	char *title = "Board Setting";
	boardSetting = new_window(title, WIN_W, WIN_H, 2);

	TreeNode *board_panel = new_panel(boardSetting, "board_panel", 0, 0, 600, WIN_H, BOARD_SIZE * BOARD_SIZE, "pics/board.bmp");
	Panel *p_board = (Panel*)board_panel->control;
	TreeNode *menu_panel = new_panel(boardSetting, "menu_panel", 600, 0, 200, WIN_H, 4, NULL);
	Panel *p_menu = (Panel*)menu_panel->control;
	//TreeNode *piece23 = new_button(board_panel, "piece_to_pick", 40, 40, TILE, TILE, 0, "pics/king_b.bmp", NULL, NULL);

	TreeNode *logo = new_label(menu_panel, "logo", p_menu->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *ok = new_button(menu_panel, "OK", p_menu->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/OK.bmp", board_set_ok, NULL);
	TreeNode *cancel = new_button(menu_panel, "cancel", p_menu->width / 2 - BUTTON_W / 2, 30 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, NULL);
	TreeNode *tiles_panel = new_panel(menu_panel, "tiles_panel", 600, 100 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 12, NULL);
	Panel *p_tiles = (Panel*)tiles_panel->control;
	
	TreeNode *piece11 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 0, TILE, TILE, 0, "pics/pawn_w.bmp", set_piece_picked, WHITE_P);
	TreeNode *piece21 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, TILE, TILE, TILE, 0, "pics/rook_w.bmp", set_piece_picked, WHITE_R);
	TreeNode *piece31 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 2 * TILE, TILE, TILE, 0, "pics/bishop_w.bmp", set_piece_picked, WHITE_B);
	TreeNode *piece41 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 3 * TILE, TILE, TILE, 0, "pics/knight_w.bmp", set_piece_picked, WHITE_N);
	TreeNode *piece51 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 4 * TILE, TILE, TILE, 0, "pics/queen_w.bmp", set_piece_picked, WHITE_Q);
	TreeNode *piece61 = new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 5 * TILE, TILE, TILE, 0, "pics/king_w.bmp", set_piece_picked, WHITE_K);
	TreeNode *piece12 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 0, TILE, TILE, 0, "pics/pawn_b.bmp", set_piece_picked, BLACK_P);
	TreeNode *piece22 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, TILE, TILE, TILE, 0, "pics/rook_b.bmp", set_piece_picked, BLACK_R);
	TreeNode *piece32 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 2 * TILE, TILE, TILE, 0, "pics/bishop_b.bmp", set_piece_picked, BLACK_B);
	TreeNode *piece42 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 3 * TILE, TILE, TILE, 0, "pics/knight_b.bmp", set_piece_picked, BLACK_N);
	TreeNode *piece52 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 4 * TILE, TILE, TILE, 0, "pics/queen_b.bmp", set_piece_picked, BLACK_Q);
	TreeNode *piece62 = new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 5 * TILE, TILE, TILE, 0, "pics/king_b.bmp", set_piece_picked, BLACK_K);

	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++){
		new_button(board_panel, "tile_btn", 40 + TILE * i, 40 + TILE * (BOARD_SIZE - 1 - j), TILE, TILE, 0, NULL, tile_clicked, i * 10 + j);
		}
	//char *btn_pic = malloc(sizeof(char) * 15);
	////int *arg;
	//for (int i = 0; i < SAVE_SLOTS; i++){
	//	sprintf_s(btn_pic, 15, "pics/slot%d.bmp", i);
	//	//arg = malloc(sizeof(int)); // for compatibility - button args must be dynamicly allocated
	//	//*arg = i;
	//	new_button(panel, "slot_btn", p->width / 2 - BUTTON_W / 2, 70 + (BUTTON_H + 10) * i, BUTTON_W, BUTTON_H, 0, btn_pic, func, i);
	//}
	//free(btn_pic);

	update_board_gui(board_panel, curr_board);
	duplicate_board(curr_board, tmp_board);

	draw_tree(boardSetting);
}

void open_board_setting(){
	prevScreen = currScreen;
	init_board_setting();
	currScreen = boardSetting;
}

void init_player_selection(){
	playerSelection = new_window("Player Selection", WIN_W / 2, (WIN_H * 2) / 3, 1);
	TreeNode *panel = new_panel(playerSelection, "selection_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 10, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *logo = new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");

	TreeNode *player = new_label(panel, "player_logo", (p->width / 4) - 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *p_ai = new_button(panel, "PvsAI", p->width / 2 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsAI.bmp", set_player, 1);
	TreeNode *p_p = new_button(panel, "PvsP", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsP.bmp", set_player, 2);

	TreeNode *next = new_label(panel, "next_logo", (p->width / 4) - 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *next_white = new_button(panel, "next_white", p->width / 2 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_white.bmp", set_next, WHITE);
	TreeNode *next_black = new_button(panel, "next_black", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_black.bmp", set_next, BLACK);

	TreeNode *set_board = new_button(panel, "set_board", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H * 2, BUTTON_W, BUTTON_H, 0, "pics/set_board.bmp", open_board_setting, NULL);

	TreeNode *start = new_button(panel, "start", p->width - 20 - BUTTON_W, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/start.bmp", cancel_clicked, NULL);
	TreeNode *cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, NULL);
	
	if (!board_ready) init_board(curr_board);
	draw_tree(playerSelection);
}

void open_player_selection(){
	prevScreen = currScreen;
	init_player_selection();
	currScreen = playerSelection;
}


void load_slot(int slot){
	char file[16] = "slots/game0.xml";
	file[10] = slot + '0';
	load_game(&file, curr_board);
	board_ready = 1;
	open_player_selection();
}


void save_slot(int slot){

}

void init_load_save(int load_save){
	char *title = "Load";
	char *file = "pics/Load.bmp";
	void(*func)(void*) = load_slot;
	if (load_save == 1){
		title = "Save";
		file = "pics/Save.bmp";
		func = save_slot;
	}
	loadSave = new_window(title, WIN_W / 2, WIN_H, 1);

	TreeNode *panel = new_panel(loadSave, "ls_panel", 0, 0, WIN_W / 2, WIN_H, SAVE_SLOTS + 2, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *logo = new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, file);
	char *btn_pic = malloc(sizeof(char) * 15);
	//int *arg;
	for (int i = 0; i < SAVE_SLOTS; i++){
		sprintf_s(btn_pic, 15, "pics/slot%d.bmp", i);
		//arg = malloc(sizeof(int)); // for compatibility - button args must be dynamicly allocated
		//*arg = i;
		new_button(panel, "slot_btn", p->width / 2 - BUTTON_W / 2, 70 + (BUTTON_H + 10) * i, BUTTON_W, BUTTON_H, 0, btn_pic, func, i);
	}
	free(btn_pic);

	TreeNode *cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, NULL);

	draw_tree(loadSave);
}

void open_load_save(int i){
	prevScreen = currScreen;
	init_load_save(i);
	currScreen = loadSave;
}

void init_main_menu(){
	mainMenu = new_window("Chess Main Menu", WIN_W / 2, (WIN_H * 2) / 3, 1);
	TreeNode *panel = new_panel(mainMenu, "menu_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 4, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *logo = new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *button1 = new_button(panel, "new", p->width / 2 - BUTTON_W / 2, 100, BUTTON_W, BUTTON_H, 0, "pics/new_game.bmp", open_player_selection, NULL);
	TreeNode *button2 = new_button(panel, "load", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/load_game.bmp", open_load_save, 0);
	TreeNode *button3 = new_button(panel, "quit", p->width / 2 - BUTTON_W / 2, 120 + (BUTTON_H * 2), BUTTON_W, BUTTON_H, 0, "pics/quit.bmp", quit_all, NULL);

	draw_tree(mainMenu);
}


int main_sdl(void) {

	//init_board_setting();
	//currScreen = boardSetting;
	init_main_menu();
	currScreen = mainMenu;
	while (!glob_quit)
		run_events_loop(currScreen);

	return 0;
}









//int main_sdl(void) {
//
//	init_main_menu();
//	currScreen = mainMenu;
//	while (!glob_quit)
//		run_events_loop(currScreen);
//	//SDL_Surface *w = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
//	//test_animate(w);
//	//main_menu(w);
//	//atexit(SDL_Quit);
//	return 0;
//}
//
//int game_screen(){}
//
//int settings_screen(){}
//
//int main_menu(SDL_Surface *w){
//	SDL_Event e;
//	SDL_Rect buttons[3] = { { WIN_W / 2 - BUTTON_W / 2, 110, BUTTON_W, BUTTON_H },
//	{ WIN_W / 2 - BUTTON_W / 2, 120 + BUTTON_H, BUTTON_W, BUTTON_H },
//	{ WIN_W / 2 - BUTTON_W / 2, 130 + BUTTON_H * 2, BUTTON_W, BUTTON_H } };
//
//	if (SDL_FillRect(w, 0, 0) != 0) {
//		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
//		return 1;
//	}
//
//	foreach(SDL_Rect *button, buttons){
//		if (SDL_FillRect(w, button, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
//			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
//			return 1;
//		}
//	}
//
//	SDL_Surface *dummy = SDL_LoadBMP("/Users/liadwg/Documents/visual studio 2013/Projects/ChessProg/pics/button1.bmp");
//	SDL_BlitSurface(dummy, NULL, w, &buttons[0]);
//	if (SDL_SetColorKey(w, SDL_SRCCOLORKEY, SDL_MapRGB(dummy->format, 0, 255, 0)) != 0) {
//		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
//		SDL_FreeSurface(dummy);
//		return 1;
//	}
//
//
//	/* We finished drawing*/
//	if (SDL_Flip(w) != 0) {
//		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
//		return 1;
//	}
//
//	/* Poll for keyboard & mouse events*/
//	while (!glob_quit) {
//		while (SDL_PollEvent(&e) != 0) {
//			switch (e.type) {
//			case (SDL_QUIT) :
//				glob_quit = 1;
//				break;
//			case (SDL_KEYUP) :
//				if (e.key.keysym.sym == SDLK_ESCAPE) glob_quit = 1;
//				break;
//			case (SDL_MOUSEBUTTONUP) :
//				if ((e.button.x > buttons[2].x) && (e.button.x < buttons[2].x + buttons[2].w) && (e.button.y > buttons[2].y) && (e.button.y < buttons[2].y + buttons[2].h))
//					glob_quit = 1;
//				break;
//			default:
//				break;
//			}
//		}
//	}
//	return 0;
//}
//
//int test_animate(SDL_Surface *w){
//	SDL_Event e;
//	SDL_Rect rect = { 10, 10, 50, 50 };
//	SDL_Rect imgrect = { 0, 0, IMG_W, IMG_H };
//	SDL_Surface *img = SDL_LoadBMP("/Users/liadwg/Documents/visual studio 2013/Projects/ChessProg/test2.bmp");
//	int quit = 0;
//
//	/* Initialize SDL and make sure it quits*/
//	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
//		return 1;
//	}
//	//atexit(SDL_Quit);
//
//	/* Define the rects we need*/
//
//
//	/* Load test spritesheet image*/
//
//	if (img == NULL) {
//		printf("ERROR: failed to load image: %s\n", SDL_GetError());
//		return 1;
//	}
//
//	/* Set colorkey to BLUE*/
//	if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 255)) != 0) {
//		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
//		SDL_FreeSurface(img);
//		return 1;
//	}
//
//
//	while (!quit) {
//		/* Clear window to BLACK*/
//		if (SDL_FillRect(w, 0, 0) != 0) {
//			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
//			break;
//		}
//
//		/* Green rectangle button*/
//		if (SDL_FillRect(w, &rect, SDL_MapRGB(w->format, 0, 255, 0)) != 0) {
//			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
//			break;
//		}
//
//		/* Draw image sprite*/
//		if (SDL_BlitSurface(img, &imgrect, w, 0) != 0) {
//			SDL_FreeSurface(img);
//			printf("ERROR: failed to blit image: %s\n", SDL_GetError());
//			break;
//		}
//
//		/* Advance to next sprite*/
//		imgrect.x += imgrect.w;
//		if (imgrect.x >= img->w) {
//			imgrect.x = 0;
//			imgrect.y += imgrect.h;
//			if (imgrect.y >= img->h) imgrect.y = 0;
//		}
//
//		/* We finished drawing*/
//		if (SDL_Flip(w) != 0) {
//			printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
//			break;
//		}
//
//		/* Poll for keyboard & mouse events*/
//
//		while (SDL_PollEvent(&e) != 0) {
//			switch (e.type) {
//			case (SDL_QUIT) :
//				quit = 1;
//				break;
//			case (SDL_KEYUP) :
//				if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
//				break;
//			case (SDL_MOUSEBUTTONUP) :
//				if ((e.button.x > rect.x) && (e.button.x < rect.x + rect.w) && (e.button.y > rect.y) && (e.button.y < rect.y + rect.h))
//					quit = 1;
//				break;
//			default:
//				break;
//			}
//		}
//
//		/* Wait a little before redrawing*/
//		SDL_Delay(10);
//	}
//
//	SDL_FreeSurface(img);
//	atexit(SDL_Quit);
//	return 0;
//}
