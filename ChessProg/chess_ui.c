#include "chess_ui.h"

#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

int glob_quit = 0;
int start_game = 0;
int board_ready = 0;
char piece_picked = NULL;
TreeNode *mainMenu = NULL;
TreeNode *loadSave = NULL;
TreeNode *AIsettingsMenu = NULL;
TreeNode *playerSelection = NULL;
TreeNode *boardSetting = NULL;
TreeNode *gameWindow = NULL;
TreeNode *prevScreen = NULL;
TreeNode *currScreen = NULL;
char gui_board[BOARD_SIZE][BOARD_SIZE];
char tmp_board[BOARD_SIZE][BOARD_SIZE];
Pos *src_pos = NULL;
Move *move_to_do = NULL;
int wait4promote = 0;
TreeNode *tmp_panel = NULL;


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
	if (wait4promote){
		move_to_do->promote = piece;
		wait4promote = 0;
		TreeNode *menu = gameWindow->children[1]; // menu panel
		free_tree(menu->children[2]);
		menu->children[2] = tmp_panel;
		tmp_panel = NULL;
	}
	else piece_picked = piece;
}

void show_best_move(int depth){
	if (depth == 0) depth = minimax_depth;
	Move *move = get_best_move(gui_board, depth);
	TreeNode *n1 = get_button_node(gameWindow->children[0], move->piece.col * 10 + move->piece.row)->children[0];
	TreeNode *n2 = get_button_node(gameWindow->children[0], move->dest.col * 10 + move->dest.row);
	if (n2->child_num != 0) n2 = n2->children[0];
	
	n1->child_num = 1;
	n1->children = malloc(sizeof(TreeNode*));
	n1->children[0] = NULL;
	n2->child_num = 1;
	n2->children[0] = NULL;
	n2->children = malloc(sizeof(TreeNode*));

	TreeNode *l1 = new_label(n1, "highlight", ((Label*)n1->control)->x, ((Label*)n1->control)->y, TILE, TILE, 0, "pics/highlight_tile.bmp");
	TreeNode *l2 = new_label(n2, "highlight", ((Label*)n2->control)->x, ((Label*)n2->control)->y, TILE, TILE, 0, "pics/highlight_tile.bmp");

	//realloc(gameWindow->children, sizeof(TreeNode*) * ++gameWindow->child_num);
	//gameWindow->children[gameWindow->child_num - 1] = NULL;
	//TreeNode *alert = new_panel(gameWindow, "alert panel", WIN_W / 2 - 300, WIN_H / 2 - 150, 600, 300, 0, pic);
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
	//realloc(gameWindow->children, sizeof(TreeNode*) * --gameWindow->child_num);
	draw_tree(gameWindow);
}

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

void tile_clicked(int tile){
	TreeNode *tile_node = get_button_node(currScreen, tile);
	Button *button = (Button*)tile_node->control;
	if (currScreen == boardSetting){
		if (piece_picked == NULL && tile_node->child_num == 0) return;
		else if (piece_picked != NULL){
			tmp_board[button->args / 10][button->args % 10] = piece_picked;
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

void screen_dismissed(TreeNode *screen){
	if (screen == mainMenu) mainMenu = NULL;
	else if (screen == loadSave) loadSave = NULL;
	else if (screen == AIsettingsMenu) AIsettingsMenu = NULL;
	else if (screen == playerSelection) playerSelection = NULL;
	else if (screen == boardSetting) boardSetting = NULL;
	else if (screen == gameWindow) gameWindow = NULL;
}

void cancel_clicked(){
	if (currScreen == prevScreen) prevScreen = mainMenu; // make sure its ok in all flows
	piece_picked = NULL;
	free_tree(currScreen);
	screen_dismissed(currScreen);
	draw_tree(prevScreen);
	currScreen = prevScreen;
}

void set_depth(int i){
	minimax_depth = i;
	cancel_clicked();
}

void init_AI_setting(){
	AIsettingsMenu = new_window("Set Difficulty", WIN_W / 2, WIN_H, 1);

	TreeNode *panel = new_panel(AIsettingsMenu, "AI_panel", 0, 0, WIN_W / 2, WIN_H, 5, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *d1 = new_button(panel, "depth1", p->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth1.bmp", set_depth, 1);
	TreeNode *d2 = new_button(panel, "depth2", p->width / 2 - BUTTON_W / 2, 30 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth2.bmp", set_depth, 2);
	TreeNode *d3 = new_button(panel, "depth3", p->width / 2 - BUTTON_W / 2, 40 + 3 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth3.bmp", set_depth, 3);
	TreeNode *d4 = new_button(panel, "depth4", p->width / 2 - BUTTON_W / 2, 50 + 4 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth4.bmp", set_depth, 4);
	TreeNode *d_best = new_button(panel, "best_depth", p->width / 2 - BUTTON_W / 2, 60 + 5 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_depth.bmp", set_depth, -1);

	draw_tree(AIsettingsMenu);
}

void set_player(int i){
	game_mode = i;
	if (i == 2){
		prevScreen = currScreen;
		init_AI_setting();
		currScreen = AIsettingsMenu;
	}
}

void set_next(COLOR i){
	start_color = i;
}

void board_set_ok(){
	if (!is_valid_board(tmp_board)) return;
	duplicate_board(tmp_board, gui_board);
	cancel_clicked();
}

void open_main_menu(){
	prevScreen = currScreen;
	init_main_menu();
	currScreen = mainMenu;
}

void init_promote_view(){
	wait4promote = 1;
	TreeNode *menu = gameWindow->children[1]; // menu panel
	tmp_panel = menu->children[2]; // best move panel
	menu->children[2] = NULL;
	TreeNode *promote_panel = new_panel(menu, "promote_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 5, NULL);
	Panel *p_promote = (Panel*)promote_panel->control;
	TreeNode *promote_logo = new_label(promote_panel, "promote_logo", p_promote->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/promote_inst.bmp");
	if (curr_player == WHITE){
		TreeNode *piece21 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H, TILE, TILE, 0, "pics/rook_w.bmp", set_piece_picked, WHITE_R);
		TreeNode *piece31 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + TILE, TILE, TILE, 0, "pics/bishop_w.bmp", set_piece_picked, WHITE_B);
		TreeNode *piece41 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 2 * TILE, TILE, TILE, 0, "pics/knight_w.bmp", set_piece_picked, WHITE_N);
		TreeNode *piece51 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 3 * TILE, TILE, TILE, 0, "pics/queen_w.bmp", set_piece_picked, WHITE_Q);
	}
	else{
		TreeNode *piece22 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H, TILE, TILE, 0, "pics/rook_b.bmp", set_piece_picked, BLACK_R);
		TreeNode *piece32 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + TILE, TILE, TILE, 0, "pics/bishop_b.bmp", set_piece_picked, BLACK_B);
		TreeNode *piece42 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 2 * TILE, TILE, TILE, 0, "pics/knight_b.bmp", set_piece_picked, BLACK_N);
		TreeNode *piece52 = new_button(promote_panel, "piece_to_pick", p_promote->width / 2 - TILE / 2, BUTTON_H + 3 * TILE, TILE, TILE, 0, "pics/queen_b.bmp", set_piece_picked, BLACK_Q);
	}

	draw_tree(gameWindow);
}

void init_game_window(){
	char *title = "CHESS GAME";
	gameWindow = new_window(title, WIN_W, WIN_H, 2);

	TreeNode *board_panel = new_panel(gameWindow, "board_panel", 0, 0, 600, WIN_H, BOARD_SIZE * BOARD_SIZE, "pics/board.bmp");
	Panel *p_board = (Panel*)board_panel->control;
	TreeNode *menu_panel = new_panel(gameWindow, "menu_panel", 600, 0, 200, WIN_H, 3, NULL);
	Panel *p_menu = (Panel*)menu_panel->control;

	TreeNode *save = new_button(menu_panel, "save", p_menu->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/save_game.bmp", open_load_save, 1);
	TreeNode *menu = new_button(menu_panel, "menu", p_menu->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/main_menu.bmp", open_main_menu, NULL);
	if (game_mode == 1){ // PvsP mode
		TreeNode *best_panel = new_panel(menu_panel, "best_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 6, NULL);
		Panel *p_best = (Panel*)best_panel->control;

		TreeNode *best_logo = new_label(best_panel, "best_logo", p_best->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/best_logo.bmp");
		TreeNode *d1 = new_button(best_panel, "depth1", p_best->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth1.bmp", show_best_move, 1);
		TreeNode *d2 = new_button(best_panel, "depth2", p_best->width / 2 - BUTTON_W / 2, 30 + 2 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth2.bmp", show_best_move, 2);
		TreeNode *d3 = new_button(best_panel, "depth3", p_best->width / 2 - BUTTON_W / 2, 40 + 3 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth3.bmp", show_best_move, 3);
		TreeNode *d4 = new_button(best_panel, "depth4", p_best->width / 2 - BUTTON_W / 2, 50 + 4 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/depth4.bmp", show_best_move, 4);
		TreeNode *d_best = new_button(best_panel, "best_depth", p_best->width / 2 - BUTTON_W / 2, 60 + 5 * BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_depth.bmp", show_best_move, -1);
	}
	else{ // PvsAI mode
		TreeNode *best_panel = new_panel(menu_panel, "best_panel", 600, 40 + 2 * BUTTON_H, 200, (WIN_H - 100 + 2 * BUTTON_H), 2, NULL);
		Panel *p_best = (Panel*)best_panel->control;

		TreeNode *best_logo = new_label(best_panel, "best_logo", p_best->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/best_logo.bmp");
		TreeNode *d = new_button(best_panel, "best_move", p_best->width / 2 - BUTTON_W / 2, 20 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/best_move.bmp", show_best_move, 0);
	}

	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++){
		new_button(board_panel, "tile_btn", 40 + TILE * i, 40 + TILE * (BOARD_SIZE - 1 - j), TILE, TILE, 0, NULL, tile_clicked, i * 10 + j);
		}

	update_board_gui(board_panel, gui_board);

	draw_tree(gameWindow);
}


void init_board_setting(){
	char *title = "Board Setting";
	boardSetting = new_window(title, WIN_W, WIN_H, 2);

	TreeNode *board_panel = new_panel(boardSetting, "board_panel", 0, 0, 600, WIN_H, BOARD_SIZE * BOARD_SIZE, "pics/board.bmp");
	Panel *p_board = (Panel*)board_panel->control;
	TreeNode *menu_panel = new_panel(boardSetting, "menu_panel", 600, 0, 200, WIN_H, 4, NULL);
	Panel *p_menu = (Panel*)menu_panel->control;

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

	update_board_gui(board_panel, gui_board);
	duplicate_board(gui_board, tmp_board);

	draw_tree(boardSetting);
}

void open_board_setting(){
	prevScreen = currScreen;
	init_board_setting();
	currScreen = boardSetting;
}

void start_game_clicked(){
	start_game = 1;
	//if (mainMenu != NULL) free_tree(mainMenu);
	if (loadSave != NULL) free_tree(loadSave);
	loadSave = NULL;
	if (AIsettingsMenu != NULL) free_tree(AIsettingsMenu);
	AIsettingsMenu = NULL;
	if (playerSelection != NULL) free_tree(playerSelection);
	playerSelection = NULL;
	if (boardSetting != NULL) free_tree(boardSetting);
	boardSetting = NULL;
}

void init_player_selection(){
	playerSelection = new_window("Player Selection", WIN_W / 2, (WIN_H * 2) / 3, 1);
	TreeNode *panel = new_panel(playerSelection, "selection_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 10, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *logo = new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");

	TreeNode *player = new_label(panel, "player_logo", (p->width / 4) - 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *p_ai = new_button(panel, "PvsAI", p->width / 2 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsAI.bmp", set_player, 2);
	TreeNode *p_p = new_button(panel, "PvsP", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 70, BUTTON_W, BUTTON_H, 0, "pics/PvsP.bmp", set_player, 1);

	TreeNode *next = new_label(panel, "next_logo", (p->width / 4) - 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *next_white = new_button(panel, "next_white", p->width / 2 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_white.bmp", set_next, WHITE);
	TreeNode *next_black = new_button(panel, "next_black", (p->width / 4) * 3 + 10 - BUTTON_W / 2, 90 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/next_black.bmp", set_next, BLACK);

	TreeNode *set_board = new_button(panel, "set_board", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H * 2, BUTTON_W, BUTTON_H, 0, "pics/set_board.bmp", open_board_setting, NULL);

	TreeNode *start = new_button(panel, "start", p->width - 20 - BUTTON_W, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/start.bmp", start_game_clicked, NULL);
	TreeNode *cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, NULL);
	
	if (!board_ready) init_board(gui_board);
	draw_tree(playerSelection);
}

void open_player_selection(){
	prevScreen = currScreen;
	init_player_selection();
	currScreen = playerSelection;
}


void load_slot(int slot){
	start_game = 0;
	char file[16] = "slots/game0.xml";
	file[10] = slot + '0';
	load_game(&file, gui_board);
	board_ready = 1;
	open_player_selection();
}


void save_slot(int slot){
	char file[16] = "slots/game0.xml";
	file[10] = slot + '0';
	save_game(gui_board, curr_player, &file);
	cancel_clicked();
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
	for (int i = 0; i < SAVE_SLOTS; i++){
		sprintf_s(btn_pic, 15, "pics/slot%d.bmp", i);
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
	TreeNode *panel = new_panel(mainMenu, "menu_panel", 0, 0, WIN_W / 2, (WIN_H * 2) / 3, 5, NULL);
	Panel *p = (Panel*)panel->control;

	TreeNode *logo = new_label(panel, "logo", p->width / 2 - BUTTON_W / 2, 10, BUTTON_W, BUTTON_H, 0, "pics/logo.bmp");
	TreeNode *button1 = new_button(panel, "new", p->width / 2 - BUTTON_W / 2, 100, BUTTON_W, BUTTON_H, 0, "pics/new_game.bmp", open_player_selection, NULL);
	TreeNode *button3 = new_button(panel, "quit", p->width / 2 - BUTTON_W / 2, 120 + (BUTTON_H * 2), BUTTON_W, BUTTON_H, 0, "pics/quit.bmp", quit_all, NULL);
	TreeNode *button2;
	TreeNode *cancel;
	if (start_game){
		cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/cancel.bmp", cancel_clicked, NULL);
		button2 = new_button(panel, "load", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H, BUTTON_W, BUTTON_H, 0, NULL, NULL, NULL);
	}
	else{
		cancel = new_button(panel, "cancel", 20, p->height - 20 - BUTTON_H, BUTTON_W, BUTTON_H, 0, NULL, NULL, NULL);
		button2 = new_button(panel, "load", p->width / 2 - BUTTON_W / 2, 110 + BUTTON_H, BUTTON_W, BUTTON_H, 0, "pics/load_game.bmp", open_load_save, 0);
	}

	draw_tree(mainMenu);
}

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
		if (move_to_do != NULL && !wait4promote) return move_to_do;
		run_events_loop(currScreen);
	}
}


int gui_setting_mode(){

	init_main_menu();
	currScreen = mainMenu;
	while (!glob_quit){
		if (start_game) return 1;
		run_events_loop(currScreen);
	}

	return 0;
}