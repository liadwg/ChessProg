#include "ChessProg.h"

void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE * 4; i++){
		printf("-");
	}
	printf("|\n");
}

void print_board(char board[BOARD_SIZE][BOARD_SIZE])
{
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--)
	{
		printf((j < (BOARD_SIZE + 1) ? " %d" : "%d"), j + 1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j]);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}

// clears the board from all pieces
void clear_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			board[i][j] = EMPTY;
		}
	}
}

void init_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		board[i][1] = BLACK_P;
		board[i][BOARD_SIZE - 1] = WHITE_P;
		for (j = 2; j < (BOARD_SIZE - 2); j++) board[i][j] = EMPTY;
		switch (i){
		case 0: case (BOARD_SIZE - 1) :
			board[i][0] = BLACK_R;
			board[i][BOARD_SIZE - 1] = WHITE_R;
		case 1: case (BOARD_SIZE - 2) :
			board[i][0] = BLACK_N;
			board[i][BOARD_SIZE - 1] = WHITE_N;
		case 2: case (BOARD_SIZE - 3) :
			board[i][0] = BLACK_B;
			board[i][BOARD_SIZE - 1] = WHITE_B;
		case 3:
			board[i][0] = BLACK_Q;
			board[i][BOARD_SIZE - 1] = WHITE_Q;
		case 4:
			board[i][0] = BLACK_K;
			board[i][BOARD_SIZE - 1] = WHITE_K;
		}
	}
}

int piece2type(char * piece){
	if (piece == NULL) return 0;
	if (strcmp(piece, "queen") == 0) return 1;
	if (strcmp(piece, "bishop") == 0) return 2;
	if (strcmp(piece, "rook") == 0) return 3;
	if (strcmp(piece, "knight") == 0) return 4;
}

int load_game(char * path, char board[BOARD_SIZE][BOARD_SIZE]){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	int line_num = 0;
	if ((doc = xmlReadFile(path, NULL, 0)) == NULL) return 0;
	root_element = xmlDocGetRootElement(doc);
	xmlNode *cur_node = NULL;
	for (cur_node = root_element; cur_node; cur_node = cur_node->next) {
		if (/*cur_node->type == XML_ELEMENT_NODE &&*/ strcmp(cur_node->name, "game") == 0) {
			cur_node = cur_node->children;
			for (cur_node = root_element; cur_node; cur_node = cur_node->next, line_num++){
				if (cur_node->content == NULL) continue;
				switch (line_num){
				case 0: if (strcmp(cur_node->content, "White") == 0){} // change ? // next_turn
				case 1: game_mode = atoi(cur_node->content); // game_mode
				case 2: // difficulty
					if (strcmp(cur_node->content, "best" == 0)) minimax_depth = cmp_best(board);
					else minimax_depth = atoi(cur_node->content);
				case 3: user_color = strcmp(cur_node->content, "White") == 0 ? WHITE : BLACK; // user_color
				case 4: //board
					cur_node = cur_node->children;
					int j = 0;
					clear_board(board);
					for (cur_node = root_element; cur_node; cur_node = cur_node->next, j++){
						char * row = cur_node->content;
						for (int i = 0; i < BOARD_SIZE; i++){
							if (row[i] == '_') board[i][j] = EMPTY;
							else board[i][j] = row[i];
						}
					}
				}
			}
		}
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 1;
}

int save_game(char board[BOARD_SIZE][BOARD_SIZE], COLOR color, char * file_name){
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, board_node = NULL;/* node pointers */

	LIBXML_TEST_VERSION;

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "game");
	xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "next_turn", BAD_CAST color);
	xmlNewChild(root_node, NULL, BAD_CAST "game_mode", BAD_CAST game_mode);

	if (game_mode == 2){
		xmlNewChild(root_node, NULL, BAD_CAST "difficulty", BAD_CAST minimax_depth);
		xmlNewChild(root_node, NULL, BAD_CAST "user_color", BAD_CAST user_color);
	}
	board_node = xmlNewChild(root_node, NULL, BAD_CAST "board", NULL);
	for (int i = 0; i < BOARD_SIZE; i++){
		char * line = malloc(8);
		for (int j = 0; j < BOARD_SIZE; j++){
			if (board[i][j] == EMPTY) line[j] = '_';
			else line[j] = board[i][j];
		}
		xmlNewChild(board_node, NULL, BAD_CAST("row_%d", i), BAD_CAST line);
	}
	if (xmlSaveFormatFileEnc(file_name, doc, "UTF-8", 1) == -1) return 0;
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 1;
}

// handles user input (unknown length), returns a string without redundant white spaces after each new line
char* input2str(FILE* pFile){
	char *str;
	char ch, pch;
	size_t size = 10;
	size_t len = 0;
	str = malloc(sizeof(char)*size);
	ch = fgetc(pFile);
	pch = '~';
	while (ch != EOF && ch != '\n')
	{
		if ((pch != '~' && pch != ' ') || (ch != ' ')){
			str[len++] = ch;
			if (len == size)
			{
				size = 2 * size;
				str = realloc(str, sizeof(char)*size);
			}
			pch = ch;
			ch = fgetc(pFile);
		}
		else{
			pch = ch;
			ch = fgetc(pFile);
		}
	}
	str[len++] = '\0';
	str = realloc(str, sizeof(char)*len);
	return str;
}

// settings state input loop - gets the user's command and handles it
void exc(char* str, char board[BOARD_SIZE][BOARD_SIZE]){
	char * word1;
	word1 = strtok(str, " ");
	if (strcmp(word1, "game_mode") == 0){
		int x = atoi(strtok(NULL, " "));
		if (x < 1 || x > 2) printf(WRONG_GAME_MODE);
		else{
			game_mode = x;
			printf("Running game in %s mode\n", game_mode == 1 ? "2 players" : "player vs.AI");
		}
	}
	else if (strcmp(word1, "difficulty") == 0){ 
		char * word2 = strtok(NULL, " ");
		if (game_mode == 1) printf(WRONG_GAME_MODE); // didnt mentioned what message should be print
		else{ 
			if (strcmp(word1, "best") == 0){
				minimax_depth = 4;
				best_depth = 1;
			}
			else{
				int x = atoi(word2);
				if (x > 4 || x < 1) printf(WRONG_MINIMAX_DEPTH);
				else minimax_depth = x;
			}
		}
	}
	else if (strcmp(word1, "user_color") == 0){
		char * color = strtok(NULL, " ");
		if (game_mode == 1) printf(WRONG_GAME_MODE); // didnt mentioned what message should be print
		else if (strcmp(color, "black") == 0) user_color = BLACK;
	}
	else if (strcmp(word1, "load") == 0){
		char * path = strtok(NULL, " ");
		if (load_game(path, board)) print_board(board);
		else printf(WRONG_FILE_NAME);
	}
	else if (strcmp(word1, "clear") == 0) clear_board(board);
	else if (strcmp(word1, "next_player") == 0){
		char * color = strtok(NULL, " ");
		if (strcmp(color, "black") == 0) user_color = BLACK;
	}
	else if ((strcmp(word1, "rm") == 0) || (strcmp(word1, "set") == 0)) {
		char * coor1 = strtok(NULL, " <,>");
		char * coor2 = strtok(NULL, " <,>");
		if (coor1[0] < 'a' || coor1[0] > 'h' || atoi(coor2) < 1 || atoi(coor2) > 8) printf(WRONG_POSITION);
		if (strcmp(word1, "rm") == 0) board[coor1[0] - 'a'][atoi(coor2) - 1] = EMPTY;
		else{
			char * a = strtok(NULL, " ");
			if (a == NULL) return;
			char * b = strtok(NULL, " ");
			char piece;
			char prev_piece;
			piece = name2piece(b, a);
			prev_piece = board[coor1[0] - 'a'][atoi(coor2) - 1];
			board[coor1[0] - 'a'][atoi(coor2) - 1] = piece;
			if (!is_valid_board(board)){
				board[coor1[0] - 'a'][atoi(coor2) - 1] = prev_piece;
				printf(NO_PIECE);
			}
		}
	}
	else if (strcmp(word1, "print") == 0) print_board(board);
	else printf(ILLEGAL_COMMAND);
	return;
}

// manages the computer's turn
int computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	int ret_val;
	curr_player = color;
	alpha_beta_minimax(board, color, 0, -100, 100);
	Move * move2do = best_move;
	if (is_check(board, color) == 1){
		printf(CHECK);
		if (move2do == NULL) ret_val = WIN_POS;
	}
	else if (move2do == NULL) ret_val = TIE_POS;
	else{
		exc_move(board, move2do);
		printf("Computer: move ");
		print_move(move2do);
		print_board(board);
		ret_val = GAME_ON;
	}
	clear_old_moves(moves_head);
	return ret_val;
}

// manages the users turn, game state user input loop
int user_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	get_all_moves(board, color);
	if (is_check(board, color) == 1){
		printf(CHECK);
		if (moves_head == NULL) return WIN_POS;
	}
	if (moves_head == NULL) return TIE_POS;
	char *word1;
	char *command = NULL;
	Move* new_move = NULL;
	int ret_val;
	while (1){
		printf(ENTER_YOUR_MOVE, color);
		if (new_move != NULL) clear_old_moves(new_move);
		new_move = malloc(sizeof(Move));
		new_move->next = NULL;
		if (command != NULL) free(command);
		command = input2str(stdin);
		word1 = strtok(command, " ");
		if (strcmp(word1, "quit") == 0){
			ret_val = QUIT;
			break;
		}
		else if (strcmp(word1, "get_moves") == 0){
			char * piece_coor1 = strtok(NULL, " <,>");
			char * piece_coor2 = strtok(NULL, " <,>");
			print_piece_moves(moves_head, piece_coor1[0] - 'a', atoi(piece_coor2) - 1);
			continue;
		}
		else if (strcmp(word1, "get_best_moves") == 0){
			char * depth = strtok(command, " ");
			int prev_best_depth = best_depth;
			int prev_depth = depth;
			if (strcmp(depth, "best") == 0){
				minimax_depth = 4;
				best_depth = 1;
			}
			else{
				minimax_depth = atoi(depth);
				best_depth = 0;
			}
			alpha_beta_minimax(board, color, 0, -100, 100);
			print_best_moves(moves_head, best_move->score);
			minimax_depth = prev_depth;
			best_depth = prev_best_depth;
		}
		else if (strcmp(word1, "save") == 0){
			char * file_name = strtok(NULL, " <,>");
			FILE * file = fopen(file_name, "r");
			if (file != NULL){
				fclose;
				printf(WRONG_FILE_NAME);
			}
			else{
				save_game(board, color, file_name);
			}
		}
		else if (strcmp(word1, "move") == 0 || strcmp(word1, "get_score") == 0){
			char * piece_coor1 = strtok(NULL, " <,>");
			char * piece_coor2 = strtok(NULL, " <,>");
			new_move->piece.col = piece_coor1[0] - 'a';
			new_move->piece.row = atoi(piece_coor2) - 1;
			if (!is_valid_pos(new_move->piece)){
				printf(WRONG_POSITION);
				continue;
			}
			char * dest_coor1 = strtok(NULL, " <,>to");
			char * dest_coor2 = strtok(NULL, " <,>to");
			new_move->dest.col = dest_coor1[0] - 'a';
			new_move->dest.row = atoi(dest_coor2) - 1;
			char * piece_promote = strtok(NULL, " <,>");
			if (piece_promote == NULL){
				if ((color == BLACK && new_move->dest.row == 0) || (color == WHITE && new_move->dest.row == BOARD_SIZE - 1))
					piece_promote = "queen";
			}
			new_move->promote = piece2type(piece_promote);
			if (!is_valid_pos(new_move->dest)) printf(WRONG_POSITION);
			if (!is_valid_piece(board, new_move, color)){
				printf(NO_DISC);
				continue;
			}
			Move * move2do = is_valid_move(moves_head, new_move);
			if (move2do == NULL){
				printf(ILLEGAL_MOVE);
				continue;
			}
			else if (strcmp(word1, "move") == 0){
				exc_move(board, move2do);
				print_board(board);
				ret_val = GAME_ON;
				break;
			}
			else if (strcmp(word1, "get_score") == 0){
				printf(move2do->score);
			}
		}
		else printf(ILLEGAL_COMMAND);
	}
	free(command);
	clear_old_moves(new_move);
	clear_old_moves(moves_head);
	return ret_val;
}

int main(void)
{
	char board[BOARD_SIZE][BOARD_SIZE];
	char *command = input2str(stdin);
	int end_pos = 0;

	init_board(board);
	printf(ENTER_SETTINGS);

	while (strcmp(command, "quit") != 0){
		if (strcmp(command, "start") == 0){
			if (is_valid_board(board)) break;
			else{
				printf(WROND_BOARD_INITIALIZATION);
				free(command);
				command = input2str(stdin);
				continue;
			}
		}
		exc(command, board);
		free(command);
		command = input2str(stdin);
	}

	if (strcmp(command, "start") == 0){
		// initially we printed the board at the start of the game, commented out in order to match the running examples.
		//if (user_color == WHITE) print_board(board);
		while (1){
			if (game_mode == 2){
				if (user_color == WHITE){
					int user_ret_val = user_turn(board, WHITE);
					if (user_ret_val == QUIT) break;
					if (user_ret_val == WIN_POS || user_ret_val == TIE_POS){
						if (user_ret_val == WIN_POS) printf(BLACK_WIN);
						else printf(TIE);
						end_pos = 1;
						break;
					}
					int comp_ret_val = computer_turn(board, BLACK);
					if (comp_ret_val == WIN_POS || comp_ret_val == TIE_POS){
						if (comp_ret_val == WIN_POS) printf(WHITE_WIN);
						else printf(TIE);
						end_pos = 1;
						break;
					}
				}
				else{
					int comp_ret_val = computer_turn(board, WHITE);
					if (comp_ret_val == WIN_POS || comp_ret_val == TIE_POS){
						if (comp_ret_val == WIN_POS) printf(BLACK_WIN);
						else printf(TIE);
						end_pos = 1;
						break;
					}
					int user_ret_val = user_turn(board, BLACK);
					if (user_ret_val == QUIT) break;
					if (user_ret_val == WIN_POS || user_ret_val == TIE_POS){
						if (comp_ret_val == WIN_POS) printf(WHITE_WIN);
						else printf(TIE);
						end_pos = 1;
						break;
					}
				}
			}
			if (game_mode == 1){
				int w_ret_val = user_turn(board, WHITE);
				if (w_ret_val == QUIT) break;
				if (w_ret_val == WIN_POS || w_ret_val == TIE_POS){
					if (w_ret_val == WIN_POS) printf(BLACK_WIN);
					else printf(TIE);
					end_pos = 1;
					break;
				}
				int b_ret_val = user_turn(board, BLACK);
				if (b_ret_val == QUIT) break;
				if (b_ret_val == WIN_POS || b_ret_val == TIE_POS){
					if (b_ret_val == WIN_POS) printf(WHITE_WIN); 
					else printf(TIE);
					end_pos = 1;
					break;
				}
			}
		}
	}
	if (end_pos == 1){
		free(command);
		command = input2str(stdin);
	}
	free(command);
	//main_sdl();
	//return 0;
}


