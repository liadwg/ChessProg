#include "chess_ui.h"

// Screens Globals
TreeNode *mainMenu = NULL;
TreeNode *loadSave = NULL;
TreeNode *AIsettingsMenu = NULL;
TreeNode *playerSelection = NULL;
TreeNode *boardSetting = NULL;
TreeNode *gameWindow = NULL;
TreeNode *prevScreen = NULL;
TreeNode *currScreen = NULL;

// Game-Flow Globals
int glob_quit = 0;
int start_game = 0;
int board_ready = 0;
int back2settings = 0;

// Game Globals
char gui_board[BOARD_SIZE][BOARD_SIZE];
char tmp_board[BOARD_SIZE][BOARD_SIZE];
char piece_picked = '\0';
Pos *src_pos = NULL;
Move *move_to_do = NULL;
int wait4promote = 0;
TreeNode *tmp_panel = NULL;


// clear all UI Trees, kill screens and clear memory
void quit_all(){
	glob_quit = 1;
	game_on = 0;
	if (mainMenu != NULL) free_tree(mainMenu);
	mainMenu = NULL;
	if (loadSave != NULL) free_tree(loadSave);
	loadSave = NULL;
	if (AIsettingsMenu != NULL) free_tree(AIsettingsMenu);
	AIsettingsMenu = NULL;
	if (playerSelection != NULL) free_tree(playerSelection);
	playerSelection = NULL;
	if (boardSetting != NULL) free_tree(boardSetting);
	boardSetting = NULL;
	if (gameWindow != NULL) free_tree(gameWindow);
	gameWindow = NULL;
	atexit(SDL_Quit);
}

// attach pic to game piece and vise-versa
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
	return NULL;
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
	return '\0';
}

// helper func to avoid compelation error regarding castin from char to int
int char2int(char c){
	if (c == WHITE_K) return 0;
	if (c == WHITE_Q) return 1;
	if (c == WHITE_B) return 2;
	if (c == WHITE_R) return 3;
	if (c == WHITE_N) return 4;
	if (c == WHITE_P) return 5;
	if (c == BLACK_K) return 10;
	if (c == BLACK_Q) return 11;
	if (c == BLACK_B) return 12;
	if (c == BLACK_R) return 13;
	if (c == BLACK_N) return 14;
	if (c == BLACK_P) return 15;
}

// helper func to avoid compelation error regarding castin from char to int
char int2char(int i){
	if (i == 0) return WHITE_K;
	if (i == 1) return WHITE_Q;
	if (i == 2) return WHITE_B;
	if (i == 3) return WHITE_R;
	if (i == 4) return WHITE_N;
	if (i == 5) return WHITE_P;
	if (i == 10) return BLACK_K;
	if (i == 11) return BLACK_Q;
	if (i == 12) return BLACK_B;
	if (i == 13) return BLACK_R;
	if (i == 14) return BLACK_N;
	if (i == 15) return BLACK_P;
}

// find button node in a specipic UI tree
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

// used to alert special states on the game screen (check/mate/tie)
void alert_state(int state, COLOR player){
	char *pic = NULL;
	switch (state){
	case TIE_POS: 
		pic = "pics/tie.bmp";
		break;
	case CHECK_POS: 
		pic = "pics/check.bmp";
		break;
	case LOSE_POS:
		if (player == WHITE) pic = "pics/white_win.bmp";
		else pic = "pics/black_win.bmp";
		break;
	}

	realloc(gameWindow->children, sizeof(TreeNode*) * ++gameWindow->child_num);
	gameWindow->children[gameWindow->child_num - 1] = NULL;
	TreeNode *alert = new_panel(gameWindow, "alert panel", WIN_W / 2 - 300, WIN_H / 2 - 150, 600, 300, 0, pic);
	draw_tree(gameWindow);
	SDL_Delay(1000);
	gameWindow->child_num--;
	draw_tree(gameWindow);
	SDL_Delay(1000);
	gameWindow->child_num++;
	draw_tree(gameWindow);
	SDL_Delay(1000);
	free_tree(alert);
	realloc(gameWindow->children, sizeof(TreeNode*) * --gameWindow->child_num);
	draw_tree(gameWindow);
}

void screen_dismissed(TreeNode *screen){
	if (screen == mainMenu) mainMenu = NULL;
	else if (screen == loadSave) loadSave = NULL;
	else if (screen == AIsettingsMenu) AIsettingsMenu = NULL;
	else if (screen == playerSelection) playerSelection = NULL;
	else if (screen == boardSetting) boardSetting = NULL;
	else if (screen == gameWindow) gameWindow = NULL;
}

void restore_defaults(){
	back2settings = 0;
	glob_quit = 0;
	start_game = 0;
	user_color = WHITE;
	start_color = WHITE;
	minimax_depth = 1;
}

// attach relevant pics to the board tiles, according to the current board setting
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

/************** BUTTON HANDLERS ****************/

// respond to a piece picked from side panel (settings + promotions)
void set_piece_picked(int i){
	char piece = int2char(i);
	if (wait4promote){
		move_to_do->promote = piece;
		wait4promote = 0;
		TreeNode *menu = gameWindow->children[1]; // menu panel
		free_tree(menu->children[2]); // promote options panel
		menu->children[2] = tmp_panel;
		tmp_panel = NULL;
	}
	else piece_picked = piece;
}

// highlight the best move suggested for the player
void show_best_move(int depth){
	if (depth == 0) depth = minimax_depth;
	get_best_moves(gui_board, depth);
	
	TreeNode *n1 = get_button_node(gameWindow->children[0], best_move->piece.col * 10 + best_move->piece.row)->children[0];
	TreeNode *n2 = get_button_node(gameWindow->children[0], best_move->dest.col * 10 + best_move->dest.row);
	if (n2->child_num != 0) n2 = n2->children[0];
	
	n1->child_num = 1;
	n1->children = malloc(sizeof(TreeNode*));
	n1->children[0] = NULL;
	n2->child_num = 1;
	n2->children = malloc(sizeof(TreeNode*));
	n2->children[0] = NULL;

	TreeNode *l1 = new_label(n1, "highlight", ((Label*)n1->control)->x, ((Label*)n1->control)->y, TILE, TILE, 0, "pics/highlight_tile.bmp");
	TreeNode *l2 = new_label(n2, "highlight", ((Label*)n2->control)->x, ((Label*)n2->control)->y, TILE, TILE, 0, "pics/highlight_tile.bmp");

	draw_tree(gameWindow);
	SDL_Delay(1000);
	n1->child_num--;
	n2->child_num--;
	draw_tree(gameWindow);
	SDL_Delay(1000);
	n1->child_num++;
	n2->child_num++;
	draw_tree(gameWindow);
	SDL_Delay(1000);

	n1->child_num--;
	n2->child_num--;
	free_tree(l1);
	free_tree(l2);
	free(n1->children);
	free(n2->children);
	n1->children = NULL;
	n2->children = NULL;
	
	draw_tree(gameWindow);
}

Move* generate_move(int col, int row){
	Move *res = malloc(sizeof(Move));
	res->piece.col = src_pos->col;
	res->piece.row = src_pos->row;
	res->dest.col = col;
	res->dest.row = row;
	res->next = NULL;
	res->promote = 0;
	return res;
}

// handle a board tile click - set a piece in settings mode or generate a move representing the clicks in game mode
void tile_clicked(int tile){
	TreeNode *tile_node = get_button_node(currScreen, tile);
	Button *button = (Button*)tile_node->control;
	if (currScreen == boardSetting){
		if (piece_picked == '\0' && tile_node->child_num == 0) return;
		else if (piece_picked != '\0'){
			tmp_board[button->args / 10][button->args % 10] = piece_picked;
			piece_picked = '\0';
		}
		else{ // if (piece_picked == NULL && tile_node->child_num != 0)
			Label *lbl = (Label*)tile_node->children[0]->control;
			piece_picked = get_piece_by_pic(lbl->name);
			tmp_board[button->args / 10][button->args % 10] = EMPTY;
		}
		update_board_gui(tile_node->parent, tmp_board);
		draw_tree(currScreen);
	}
	else if (!wait4promote){ // gameWindow
		if (src_pos == NULL && tile_node->child_num == 0) return;
		else if (src_pos != NULL){
			move_to_do = generate_move(button->args / 10, button->args % 10);
			if ((gui_board[move_to_do->piece.col][move_to_do->piece.row] == WHITE_P && move_to_do->dest.row == 7) ||
				(gui_board[move_to_do->piece.col][move_to_do->piece.row] == BLACK_P && move_to_do->dest.row == 0))
				init_promote_view();
			free(src_pos);
			src_pos = NULL;
		}
		else{ // if (src_pos == NULL && tile_node->child_num != 0)
			Label *lbl = (Label*)tile_node->children[0]->control;
			if (is_opposite(curr_player, get_piece_by_pic(lbl->name))) return;
			src_pos = malloc(sizeof(Pos));
			src_pos->col = button->args / 10;
			src_pos->row = button->args % 10;
		}
	}
}

// return to previous screen
void cancel_clicked(){
	if (currScreen == prevScreen) prevScreen = mainMenu; // make sure its ok in all flows
	piece_picked = '\0';
	free_tree(currScreen);
	screen_dismissed(currScreen);
	draw_tree(prevScreen);
	currScreen = prevScreen;
}

void set_depth(int i){
	minimax_depth = i;
	cancel_clicked();
}

void set_player(int i){
	game_mode = i;
	if (i == 2){ // if set to play against AI, opens the set difficulty screen
		prevScreen = currScreen;
		init_AI_setting();
		currScreen = AIsettingsMenu;
	}
}

void set_next(COLOR i){
	start_color = i;
}

void set_player_color(COLOR i){
	user_color = i;
}

// save the new board setting
void board_set_ok(){
	if (!is_valid_board(tmp_board)) return;
	duplicate_board(tmp_board, gui_board);
	cancel_clicked();
}

void load_slot(int slot){
	start_game = 0;
	char file[16] = "slots/game0.xml";
	file[10] = slot + '0';
	load_game(file, gui_board);
	board_ready = 1;
	open_player_selection();
}

void save_slot(int slot){
	char file[16] = "slots/game0.xml";
	file[10] = slot + '0';
	save_game(gui_board, curr_player, file);
	cancel_clicked();
}

/********************** Navigation Funcs **********************/
void open_main_menu(){
	prevScreen = currScreen;
	init_main_menu();
	currScreen = mainMenu;
}

void open_board_setting(){
	prevScreen = currScreen;
	init_board_setting();
	currScreen = boardSetting;
}

void start_game_clicked(){
	start_game = 1;
	if (loadSave != NULL) free_tree(loadSave);
	loadSave = NULL;
	if (AIsettingsMenu != NULL) free_tree(AIsettingsMenu);
	AIsettingsMenu = NULL;
	if (playerSelection != NULL) free_tree(playerSelection);
	playerSelection = NULL;
	if (boardSetting != NULL) free_tree(boardSetting);
	boardSetting = NULL;
}

void open_player_selection(){
	if (start_game){ // killing game and going back to settings mode
		quit_all();
		game_on = 1;
		back2settings = 1;
		board_ready = 0;
	}
	prevScreen = currScreen;
	init_player_selection();
	currScreen = playerSelection;
}

void open_load_save(int i){
	if (start_game && i == 0){ // killing game and going back to settings mode
		quit_all();
		game_on = 1;
		back2settings = 1;
	}
	prevScreen = currScreen;
	init_load_save(i);
	currScreen = loadSave;
}

/******************************* Screen Initializers ******************************/
// opening pieces panel in the in-game side menu
void init_promote_view(){
	wait4promote = 1;
	TreeNode *menu = gameWindow->children[1]; // menu panel
	tmp_panel = menu->children[2]; // best move panel
	menu->children[2] = NULL;
	TreeNode *promote_panel = new_panel(menu, "promote_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 5, NULL);
	Panel *p_promote = (Panel*)promote_panel->control;
	new_label(promote_panel, "promote_logo", p_promote->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/promote_inst.bmp");
	if (curr_player == WHITE){
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H, TILE, TILE, 0, "pics/rook_w.bmp", set_piece_picked, char2int(WHITE_R));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + TILE, TILE, TILE, 0, "pics/bishop_w.bmp", set_piece_picked, char2int(WHITE_B));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 2 * TILE, TILE, TILE, 0, "pics/knight_w.bmp", set_piece_picked, char2int(WHITE_N));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 3 * TILE, TILE, TILE, 0, "pics/queen_w.bmp", set_piece_picked, char2int(WHITE_Q));
	}
	else{
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H, TILE, TILE, 0, "pics/rook_b.bmp", set_piece_picked, char2int(BLACK_R));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + TILE, TILE, TILE, 0, "pics/bishop_b.bmp", set_piece_picked, char2int(BLACK_B));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 2 * TILE, TILE, TILE, 0, "pics/knight_b.bmp", set_piece_picked, char2int(BLACK_N));
		new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 3 * TILE, TILE, TILE, 0, "pics/queen_b.bmp", set_piece_picked, char2int(BLACK_Q));
	}

	draw_tree(gameWindow);
}

void init_game_window(){
	if (gameWindow != NULL) free_tree(gameWindow);
	char *title = "CHESS GAME";
	gameWindow = new_window(title, WIN_W, WIN_H, 2);

	TreeNode *board_panel = new_panel(gameWindow, "board_panel", 0, 0, 600, WIN_H, BOARD_SIZE * BOARD_SIZE, "pics/board.bmp");
	Panel *p_board = (Panel*)board_panel->control;
	TreeNode *menu_panel = new_panel(gameWindow, "menu_panel", 600, 0, 200, WIN_H, 3, NULL);
	Panel *p_menu = (Panel*)menu_panel->control;

	new_button(menu_panel, "save", p_menu->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/save_game.bmp", open_load_save, 1);
	new_button(menu_panel, "menu", p_menu->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/main_menu.bmp", open_main_menu, 999);
	if (game_mode == 1){ // PvsP mode
		TreeNode *best_panel = new_panel(menu_panel, "best_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 6, NULL);
		Panel *p_best = (Panel*)best_panel->control;

		new_label(best_panel, "best_logo", p_best->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/best_logo.bmp");
		new_button(best_panel, "depth1", p_best->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth1.bmp", show_best_move, 1);
		new_button(best_panel, "depth2", p_best->width / 2 - BUTTON_W / 2, 30 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth2.bmp", show_best_move, 2);
		new_button(best_panel, "depth3", p_best->width / 2 - BUTTON_W / 2, 40 + 3 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth3.bmp", show_best_move, 3);
		new_button(best_panel, "depth4", p_best->width / 2 - BUTTON_W / 2, 50 + 4 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth4.bmp", show_best_move, 4);
		new_button(best_panel, "best_depth", p_best->width / 2 - BUTTON_W / 2, 60 + 5 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_depth.bmp", show_best_move, -1);
	}
	else{ // PvsAI mode
		TreeNode *best_panel = new_panel(menu_panel, "best_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 2, NULL);
		Panel *p_best = (Panel*)best_panel->control;

		new_label(best_panel, "best_logo", p_best->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/best_logo.bmp");
		new_button(best_panel, "best_move", p_best->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_move.bmp", show_best_move, 0);
	}

	for (int i = 0; i < BOARD_SIZE; i++) // every board tile is a transparent button 
		for (int j = 0; j < BOARD_SIZE; j++){
		new_button(board_panel, "tile_btn", 40 + TILE * i, 40 + TILE * (BOARD_SIZE - 1 - j), TILE, TILE, 0, NULL, tile_clicked, i * 10 + j);
		}

	update_board_gui(board_panel, gui_board);

	draw_tree(gameWindow);
}

void init_AI_setting(){
	if (AIsettingsMenu != NULL) free_tree(AIsettingsMenu);
	AIsettingsMenu = new_window("Set Difficulty", WIN_W / 2, WIN_H, 1);

	TreeNode *panel = new_panel(AIsettingsMenu, "AI_panel", 0, 0, WIN_W / 2, WIN_H, 9, NULL);
	Panel *p = (Panel*)panel->control;
	
	new_label(panel, "next_logo", (p->width / 4) - 10 - BUTTON_W / 2, 50, BUTTON_W, BUTTON_H, 0, "pics/AI_color.bmp");
	new_button(panel, "next_white", p->width / 2 - BUTTON_W / 2, 50, BUTTON_W, BUTTON_H, 0, "pics/white.bmp", set_player_color, 1); // BLACK
	new_button(panel, "next_black", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 50, BUTTON_W, BUTTON_H, 0, "pics/black.bmp", set_player_color, 0); // WHITE

	new_label(panel, "difficulty", (p->width / 4) - 10 - BUTTON_W / 2, 70 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/set_diff.bmp");
	new_button(panel, "depth1", p->width / 2 - BUTTON_W / 2, 70 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth1.bmp", set_depth, 1);
	new_button(panel, "depth2", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 70 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth2.bmp", set_depth, 2);
	new_button(panel, "depth3", p->width / 2 - BUTTON_W / 2, 80 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth3.bmp", set_depth, 3);
	new_button(panel, "depth4", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 80 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth4.bmp", set_depth, 4);
	new_button(panel, "best_depth", p->width / 2 - BUTTON_W / 2, 90 + 3 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_depth.bmp", set_depth, -1);


	draw_tree(AIsettingsMenu);
}

void init_board_setting(){
	if (boardSetting != NULL) free_tree(boardSetting);

	char *title = "Board Setting";
	boardSetting = new_window(title, WIN_W, WIN_H, 2);

	TreeNode *board_panel = new_panel(boardSetting, "board_panel", 0, 0, 600, WIN_H, BOARD_SIZE * BOARD_SIZE, "pics/board.bmp");
	Panel *p_board = (Panel*)board_panel->control;
	TreeNode *menu_panel = new_panel(boardSetting, "menu_panel", 600, 0, 200, WIN_H, 4, NULL);
	Panel *p_menu = (Panel*)menu_panel->control;

	new_label(menu_panel, "logo", p_menu->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	new_button(menu_panel, "OK", p_menu->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/OK.bmp", board_set_ok, 999);
	new_button(menu_panel, "cancel", p_menu->width / 2 - BUTTON_W / 2, 30 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, 999);
	TreeNode *tiles_panel = new_panel(menu_panel, "tiles_panel", 600, 100 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 12, NULL);
	Panel *p_tiles = (Panel*)tiles_panel->control;
	
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 0, TILE, TILE, 0, "pics/pawn_w.bmp", set_piece_picked, char2int(WHITE_P));
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, TILE, TILE, TILE, 0, "pics/rook_w.bmp", set_piece_picked, char2int(WHITE_R));
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 2 * TILE, TILE, TILE, 0, "pics/bishop_w.bmp", set_piece_picked, char2int(WHITE_B));
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 3 * TILE, TILE, TILE, 0, "pics/knight_w.bmp", set_piece_picked, char2int(WHITE_N));
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 4 * TILE, TILE, TILE, 0, "pics/queen_w.bmp", set_piece_picked, char2int(WHITE_Q));
	new_button(tiles_panel, "piece_to_pick", p_tiles->width / 3 - TILE / 2, 5 * TILE, TILE, TILE, 0, "pics/king_w.bmp", set_piece_picked, char2int(WHITE_K));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 0, TILE, TILE, 0, "pics/pawn_b.bmp", set_piece_picked, char2int(BLACK_P));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, TILE, TILE, TILE, 0, "pics/rook_b.bmp", set_piece_picked, char2int(BLACK_R));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 2 * TILE, TILE, TILE, 0, "pics/bishop_b.bmp", set_piece_picked, char2int(BLACK_B));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 3 * TILE, TILE, TILE, 0, "pics/knight_b.bmp", set_piece_picked, char2int(BLACK_N));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 4 * TILE, TILE, TILE, 0, "pics/queen_b.bmp", set_piece_picked, char2int(BLACK_Q));
	new_button(tiles_panel, "piece_to_pick", 2 * p_tiles->width / 3 - TILE / 2, 5 * TILE, TILE, TILE, 0, "pics/king_b.bmp", set_piece_picked, char2int(BLACK_K));

	for (int i = 0; i < BOARD_SIZE; i++) // every board tile is a transparent button
		for (int j = 0; j < BOARD_SIZE; j++){
		new_button(board_panel, "tile_btn", 40 + TILE * i, 40 + TILE * (BOARD_SIZE - 1 - j), TILE, TILE, 0, NULL, tile_clicked, i * 10 + j);
		}

	update_board_gui(board_panel, gui_board);
	duplicate_board(gui_board, tmp_board);

	draw_tree(boardSetting);
}

void init_player_selection(){
	if (playerSelection != NULL) free_tree(playerSelection);

	playerSelection = new_window("Player Selection", WIN_W / 2, (WIN_H * 2) / 3, 1);
	TreeNode *panel = new_panel(playerSelection, "selection_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 10, NULL);
	Panel *p = (Panel*)panel->control;

	new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");

	new_label(panel, "player_logo", (p->width / 4) - 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/set_players.bmp");
	new_button(panel, "PvsAI", p->width / 2 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsAI.bmp", set_player, 2);
	new_button(panel, "PvsP", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsP.bmp", set_player, 1);

	new_label(panel, "next_logo", (p->width / 4) - 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_player.bmp");
	new_button(panel, "next_white", p->width / 2 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_white.bmp", set_next, 0); // WHITE
	new_button(panel, "next_black", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_black.bmp", set_next, 1); // BLACK

	new_button(panel, "set_board", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H * 2, BUTTON_W, BUTTON_H, 0, "pics/set_board.bmp", open_board_setting, 999);

	new_button(panel, "start", p->width - 20 - BUTTON_W, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/start.bmp", start_game_clicked, 999);
	new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, 999);
	
	if (!board_ready) init_board(gui_board);
	draw_tree(playerSelection);
}

void init_load_save(int load_save){
	if (loadSave != NULL) free_tree(loadSave);

	char *title = "Load";
	char *file = "pics/Load.bmp";
	void(*func)(int) = load_slot;
	if (load_save == 1){
		title = "Save";
		file = "pics/Save.bmp";
		func = save_slot;
	}
	loadSave = new_window(title, WIN_W / 2, WIN_H, 1);

	TreeNode *panel = new_panel(loadSave, "ls_panel", 0, 0, WIN_W / 2, WIN_H, SAVE_SLOTS + 2, NULL);
	Panel *p = (Panel*)panel->control;

	new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, file);
	char *btn_pic = malloc(sizeof(char) * 15);
	for (int i = 0; i < SAVE_SLOTS; i++){
		sprintf_s(btn_pic, 15, "pics/slot%d.bmp", i);
		new_button(panel, "slot_btn", p->width / 2 - BUTTON_W / 2, 70 + (BUTTON_H + 10) * i, BUTTON_W, BUTTON_H, 0, btn_pic, func, i);
	}
	free(btn_pic);

	new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, 999);

	draw_tree(loadSave);
}

void init_main_menu(){
	if (mainMenu != NULL) free_tree(mainMenu);

	mainMenu = new_window("Chess Main Menu", WIN_W / 2, (WIN_H * 2) / 3, 1);
	TreeNode *panel = new_panel(mainMenu, "menu_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 5, NULL);
	Panel *p = (Panel*)panel->control;

	new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	new_button(panel, "new", p->width / 2 - BUTTON_W / 2, 100, BUTTON_W, BUTTON_H, 0, "pics/new_game.bmp", open_player_selection, 999);
	new_button(panel, "load", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/load_game.bmp", open_load_save, 0);
	new_button(panel, "quit", p->width / 2 - BUTTON_W / 2, 120 + (BUTTON_H * 2), BUTTON_W, BUTTON_H, 0, "pics/quit.bmp", quit_all, 999);
	
	TreeNode *cancel; // cancel button - relevant only after game starts
	if (start_game) cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, 999);
	else cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, NULL, NULL, 999);

	draw_tree(mainMenu);
}
 
// main UI loop for the game mode
Move* gui_game_mode(char board[BOARD_SIZE][BOARD_SIZE]){
	if (game_on == 0){
		glob_quit = 1;
		quit_all();
	}
	duplicate_board(board, gui_board);
	move_to_do = NULL;
	if (gameWindow != NULL){
		update_board_gui(gameWindow->children[0], gui_board); // first child is the board panel
		draw_tree(gameWindow);
	}
	else init_game_window();
	currScreen = gameWindow;
	while (!glob_quit){
		if ((move_to_do != NULL && !wait4promote) || back2settings) return move_to_do;
		run_events_loop(currScreen);
	}
	return NULL;
}

// main UI loop for settings mode
int gui_setting_mode(){

	init_main_menu();
	if (currScreen == NULL) currScreen = mainMenu;
	else draw_tree(currScreen);
	restore_defaults(); // make sure default values when entering settings mode

	while (!glob_quit){
		if (start_game) return 1;
		run_events_loop(currScreen);
	}

	return 0;
}