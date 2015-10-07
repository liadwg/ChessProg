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
		board[i][1] = WHITE_P;
		board[i][BOARD_SIZE - 2] = BLACK_P;
		for (j = 2; j < (BOARD_SIZE - 2); j++) board[i][j] = EMPTY;
		switch (i){
		case 0: case (BOARD_SIZE - 1) :
			board[i][0] = WHITE_R;
			board[i][BOARD_SIZE - 1] = BLACK_R;
			break;
		case 1: case (BOARD_SIZE - 2) :
			board[i][0] = WHITE_N;
			board[i][BOARD_SIZE - 1] = BLACK_N;
			break;
		case 2: case (BOARD_SIZE - 3) :
			board[i][0] = WHITE_B;
			board[i][BOARD_SIZE - 1] = BLACK_B;
			break;
		case 3:
			board[i][0] = WHITE_Q;
			board[i][BOARD_SIZE - 1] = BLACK_Q;
			break;
		case 4:
			board[i][0] = WHITE_K;
			board[i][BOARD_SIZE - 1] = BLACK_K;
			break;
		}
	}
}

int get_line_by_node_name(char * name){
	if (strcmp(name, "next_turn") == 0) return 0;
	if (strcmp(name, "game_mode") == 0) return 1;
	if (strcmp(name, "difficulty") == 0) return 2;
	if (strcmp(name, "user_color") == 0) return 3;
	if (strcmp(name, "board") == 0) return 4;
	return -1;
}

int load_game(char * path, char board[BOARD_SIZE][BOARD_SIZE]){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	doc = xmlReadFile(path, NULL, 0);
	if (doc == NULL) return 0;
	root_element = xmlDocGetRootElement(doc);
	//xmlNode *cur_node = NULL;
	for (xmlNode * cur_node = root_element; cur_node ; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(cur_node->name, "game") == 0) {
			cur_node = cur_node->children;
			for (cur_node; cur_node; cur_node = cur_node->next){
				if (cur_node->type == XML_ELEMENT_NODE && cur_node->children != NULL){
					switch (get_line_by_node_name(cur_node->name)){
					case 0:
						if (strcmp(cur_node->children->content, "Black") == 0 ||
							strcmp(cur_node->children->content, "black") == 0) start_color = BLACK;
						else start_color = WHITE;
						break;
					case 1: 
						game_mode = atoi(cur_node->children->content);
						break;
					case 2:
						if (strcmp(cur_node->children->content, "best" == 0)){
							minimax_depth = 4; // we may want to add estimate_best_depth?
							best_depth = 1;
						}
						else  minimax_depth = atoi(cur_node->children->content);
						break;
					case 3: 
						if (strcmp(cur_node->children->content, "Black") == 0 ||
							strcmp(cur_node->children->content, "black") == 0) user_color = BLACK;
						else user_color = WHITE;
						break;
					case 4:
						clear_board(board);
						cur_node = cur_node->children;
						for (cur_node; cur_node; cur_node = cur_node->next){
							if (cur_node->type == XML_ELEMENT_NODE && cur_node->children != NULL){
								int j = (cur_node->name)[4] - '1';
								for (int i = 0; i < BOARD_SIZE; i++){
									board[i][j] = (cur_node->children->content[i] == '_') ? 
									EMPTY : cur_node->children->content[i];
								}
							}
						}
					}
				}
				if (cur_node == NULL) break; // try to do it in better way?
			}
		}
		if (cur_node == NULL) break;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 1;
}

int save_game(char board[BOARD_SIZE][BOARD_SIZE], COLOR color, char * file_name){
	int saved = 1;
	xmlTextWriterPtr writer;
	// Create a new XmlWriter
	writer = xmlNewTextWriterFilename(file_name, 0);
	if (writer == NULL) return 0;
	// Start the document
	if (xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0) saved = 0;
	if (xmlTextWriterStartElement(writer, BAD_CAST "game") < 0) saved = 0;
	if (xmlTextWriterWriteElement(writer, BAD_CAST "next_turn", start_color == BLACK ? BAD_CAST "Black" : BAD_CAST "White") < 0) saved = 0;
	if (xmlTextWriterWriteElement(writer, BAD_CAST "game_mode", game_mode == 1 ? BAD_CAST "1" : BAD_CAST "2") < 0) saved = 0;
	if (game_mode == 2) {
		if (best_depth == 1) {
			if (xmlTextWriterWriteElement(writer, BAD_CAST "difficulty", BAD_CAST "best") < 0) saved = 0;
			else {
				char depth[2];
				sprintf(depth, "%d", minimax_depth);
				if (xmlTextWriterWriteElement(writer, BAD_CAST "difficulty", BAD_CAST depth) < 0) saved = 0;
			}
			if (xmlTextWriterWriteElement(writer, BAD_CAST "user_color", user_color ? BAD_CAST "Black" : BAD_CAST "White") < 0) saved = 0;
		}

	}
	else {
		if (xmlTextWriterWriteElement(writer, BAD_CAST "difficulty", BAD_CAST "") < 0) saved = 0;
		if (xmlTextWriterWriteElement(writer, BAD_CAST "user_color", BAD_CAST "") < 0) saved = 0;
	}
	if (xmlTextWriterStartElement(writer, BAD_CAST "board") < 0) return 0;
	for (int j = BOARD_SIZE ; j > 0 ; j--) {
		char row[9];
		char row_name[6];
		for (int i = 0 ; i < BOARD_SIZE ; i++){
			row[i] = board[i][j - 1] == EMPTY ? '_' : board[i][j - 1];
			sprintf(row_name, "row_%d", j);
		}
		row[8] = '\0';
		if (xmlTextWriterWriteElement(writer, BAD_CAST row_name, BAD_CAST row) < 0) saved = 0;
	}
	if (xmlTextWriterEndDocument(writer) < 0) saved = 0;
	xmlFreeTextWriter(writer);
	xmlCleanupCharEncodingHandlers();
	xmlCleanupParser();
	return saved;
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
			printf("Running game in %s mode\n", game_mode == 1 ? "2 players" : "player vs. AI");
		}
	}
	else if (strcmp(word1, "difficulty") == 0){ 
		char * word2 = strtok(NULL, " ");
		if (game_mode == 1) printf(ILLEGAL_COMMAND);
		else{ 
			if (strcmp(word2, "best") == 0){
				minimax_depth = 4; // estimate_best_depth(board,curr_player); // check for probs
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
		if (game_mode == 1) printf(ILLEGAL_COMMAND);
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
		if (strcmp(color, "black") == 0) start_color = BLACK;
	}
	else if ((strcmp(word1, "rm") == 0) || (strcmp(word1, "set") == 0)) {
		char * coor1 = strtok(NULL, " <,>");
		char * coor2 = strtok(NULL, " <,>");
		if (coor1[0] < 'a' || coor1[0] > 'h' || atoi(coor2) < 1 || atoi(coor2) > 8) printf(WRONG_POSITION);
		if (strcmp(word1, "rm") == 0) board[coor1[0] - 'a'][atoi(coor2) - 1] = EMPTY;
		else{
			char * set_color = strtok(NULL, " ");
			if (set_color == NULL) return;
			char * set_name = strtok(NULL, " ");
			char piece2set = (strcmp(set_color,"white") == 0) ? get_piece_by_name(set_name, WHITE) : get_piece_by_name(set_name, BLACK);
			if (board[coor1[0] - 'a'][atoi(coor2) - 1] == piece2set) return; // is ok?
			int whites[6] = { 0 }, blacks[6] = { 0 };
			piece_counter(board, &whites, &blacks);
			if (get_color_by_piece(piece2set) == WHITE){
				if ((get_type_by_piece(piece2set) == 0) && whites[0] == 1) printf(WRONG_PIECE);
				else if ((get_type_by_piece(piece2set) == 1) && whites[1] == 1) printf(WRONG_PIECE);
				else if (whites[get_type_by_piece(piece2set)] == 2) printf(WRONG_PIECE);
				else board[coor1[0] - 'a'][atoi(coor2) - 1] = piece2set;
			}
			if (get_color_by_piece(piece2set) == BLACK){
				if ((get_type_by_piece(piece2set) == 0) && blacks[0] == 1) printf(WRONG_PIECE);
				if ((get_type_by_piece(piece2set) == 1) && blacks[1] == 1) printf(WRONG_PIECE);
				else if (blacks[get_type_by_piece(piece2set)] == 2) printf(WRONG_PIECE);
				else board[coor1[0] - 'a'][atoi(coor2) - 1] = piece2set;
			}
		}
	}
	else if (strcmp(word1, "print") == 0) print_board(board);
	else printf(ILLEGAL_COMMAND);
	return;
}

int pre_turn_verify(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	get_all_moves(board, color);
	if (is_check(board, color) == 1 && moves_head == NULL) return LOSE_POS;
	if (is_check(board, color) != 1 && moves_head == NULL) return TIE_POS;
	if (is_check(board, color) == 1 && moves_head != NULL) return CHECK_POS;
	return GAME_ON;
}



// manages the computer's turn
void computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	curr_player = color;
	if (moves_head != NULL){
		if (best_depth){
			minimax_depth = estimate_best_depth(board, color);
		}
		alpha_beta_minimax(board, color, 0, -500, 500);
		Move * move2do = best_move;
		exc_move(board, move2do, color);
		if (!gui_mode){
			printf("Computer: move ");
			print_move(move2do);
			print_board(board);
		}
	}
	clear_old_moves(moves_head);
}

Move * get_best_moves(){

}

// manages the users turn, game state user input loop
void user_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	curr_player = color;
	//int ret_val = GAME_ON;
	char *word1;
	char *command = NULL;
	Move* new_move = NULL;
	//get_all_moves(board, color);
	//if (is_check(board, color) == 1 && moves_head == NULL) ret_val = LOSE_POS;
	//if (is_check(board, color) != 1 && moves_head == NULL) ret_val = TIE_POS;
	//if (is_check(board, color) == 1 && moves_head != NULL) printf(CHECK);
	if (moves_head != NULL){
		while (1){
			printf(ENTER_YOUR_MOVE, color == WHITE ? "white" : "black");
			if (command != NULL) free(command);
			command = input2str(stdin);
			word1 = strtok(command, " ");

			if (strcmp(word1, "quit") == 0){
				game_on = 0;
				break;
			}

			else if (strcmp(word1, "get_moves") == 0){
				Pos pos;
				pos.col = strtok(NULL, " <,>")[0] - 'a';
				pos.row = atoi(strtok(NULL, " <,>")) - 1;
				if (!is_valid_pos(pos)) printf(WRONG_POSITION);
				else if (is_opposite(color, board[pos.col][pos.row]) || board[pos.col][pos.row] == EMPTY) printf(NO_PIECE);
				else print_piece_moves(moves_head, pos);
				continue;
			}

			else if (strcmp(word1, "get_best_moves") == 0){
				char * depth = strtok(NULL, " ");
				int prev_best_depth = best_depth;
				int prev_depth = minimax_depth;
				if (strcmp(depth, "best") == 0){
					minimax_depth = estimate_best_depth(board,color);
					best_depth = 1;
				}
				else{
					minimax_depth = atoi(depth);
					best_depth = 0;
				}
				alpha_beta_minimax(board, color, 0, -500, 500);
				int best_score = best_move->score;
				print_best_moves(moves_head, best_score);
				minimax_depth = prev_depth;
				best_depth = prev_best_depth;
				continue;
			}

			else if (strcmp(word1, "save") == 0){
				char * file_name = strtok(NULL, " <,>");
				if (save_game(board, color, file_name) != 1){
					gets(file_name);
					remove(file_name);
					printf(WRONG_FILE_NAME);
					continue;
				}
			}

			else if (strcmp(word1, "move") == 0 || strcmp(word1, "get_score") == 0){
				int prev_best_depth;
				int prev_depth;
				if (strcmp(word1, "get_score") == 0){
					char * depth = strtok(NULL, " ");
					prev_best_depth = best_depth;
					prev_depth = minimax_depth;
					if (strcmp(depth, "best") == 0){
						minimax_depth = estimate_best_depth(board,color);
						best_depth = 1;
					}
					else{
						minimax_depth = atoi(depth);
						best_depth = 0;
					}
				}
				if (new_move != NULL) clear_old_moves(new_move);
				new_move = malloc(sizeof(Move));
				new_move->next = NULL;
				new_move->piece.col = strtok(NULL, " <,>")[0] - 'a';
				new_move->piece.row = atoi(strtok(NULL, " <,>")) - 1;
				if (!is_valid_pos(new_move->piece)){
					printf(WRONG_POSITION);
					continue;
				}
				if (!is_valid_piece(board, new_move, color)){
					printf(NO_PIECE);
					continue;
				}
				new_move->dest.col = strtok(NULL, " <,>to")[0] - 'a';
				new_move->dest.row = atoi(strtok(NULL, " <,>to")) - 1;
				if (!is_valid_pos(new_move->dest)){
					printf(WRONG_POSITION);
					continue;
				}
				new_move->promote = 0;
				if (is_EOB(new_move->dest, color)){
					char * piece_promote = strtok(NULL, " <,>");
					char this_piece = board[new_move->piece.col][new_move->piece.row];
					if (this_piece == BLACK_P || this_piece == WHITE_P){
						if (piece_promote == NULL) piece_promote = "queen";
						new_move->promote = get_piece_by_name(piece_promote, color);
					}
				}
				Move * move2do = is_valid_move(moves_head, new_move);
				if (move2do == NULL){
					printf(ILLEGAL_MOVE);
					continue;
				}
				if (strcmp(word1, "get_score") == 0){
					alpha_beta_minimax(board, color, 0, -500, 500);
					move2do = is_valid_move(moves_head, move2do);
					printf("%d\n", move2do->score);
					minimax_depth = prev_depth;
					best_depth = prev_best_depth;
					continue;
				}
				if (strcmp(word1, "move") == 0){
					exc_move(board, move2do, color);
					print_board(board);
					break;
				}
			}
			else printf(ILLEGAL_COMMAND);
		}
	}
	free(command);
	clear_old_moves(new_move);
	clear_old_moves(moves_head);
}

void console_alert(int alert){
	if (alert == LOSE_POS || alert == TIE_POS){
		if (alert == LOSE_POS) printf(curr_player == WHITE ? BLACK_WIN : WHITE_WIN);
		else printf(TIE);
		game_on = 0;
	}
	else if (alert == CHECK_POS) printf(CHECK);
}

int main(int argc, char * argv[]){
	if (argc == 2) gui_mode = strcmp(argv[1], "gui") == 0 ? 1 : 0;
	//gui_mode = 1;
	char board[BOARD_SIZE][BOARD_SIZE];
	//int end_pos = 0;
	int start = 0;
	//setting state
	if (gui_mode){
		start = gui_setting_mode();
		duplicate_board(gui_board, board);
	} //setting gui
	else{ //setting console
		init_board(board);
		printf(ENTER_SETTINGS);
		char *command = input2str(stdin);
		while (strcmp(command, "quit") != 0){
			if (strcmp(command, "start") == 0){
				if (is_valid_board(board)){
					start = 1;
					break;
				}
				else printf(WROND_BOARD_INITIALIZATION); 
			}
			else exc(command, board);
			free(command);
			printf(ENTER_SETTINGS);
			command = input2str(stdin);
		}
		free(command);
	}

	if (start){
		// initially we printed the board at the start of the game, commented out in order to match the running examples.
		//if (user_color == WHITE) print_board(board);
		//print_board(board); 
		while (1){
			// user vs computer
			if (game_mode == 2){
				int turn;
				if (user_color == start_color){
					//user starts
					curr_player = user_color;
					turn = pre_turn_verify(board, curr_player);
					if (gui_mode){ //gui_mode
						Move * new_move = NULL;
						Move * move2do = NULL;
						while (move2do == NULL){
							new_move = gui_game_mode(turn, board);
							move2do = is_valid_move(moves_head, new_move);
						}
						exc_move(board, move2do, curr_player);
					}
					else{ //console mode
						console_alert(turn);
						if (game_on) user_turn(board, curr_player);
					}
					if (!game_on) break;

					curr_player = !user_color;
					turn = pre_turn_verify(board, curr_player);
					if (gui_mode){
						if (turn != GAME_ON){
							COLOR alert_color = curr_player == WHITE ? BLACK : WHITE;
							if (turn != CHECK_POS) game_on = 0;
							//alert_state(turn, alert_color); *** turn on when we have the func!
						}
						if (game_on) computer_turn(board, curr_player);
					}
					else{
						console_alert(turn);
						if (game_on) computer_turn(board, curr_player);
					}
					if (!game_on) break;
				}
				//comp starts
				else{
					curr_player = !user_color;
					turn = pre_turn_verify(board, curr_player);
					if (gui_mode){
						if (turn != GAME_ON){
							COLOR alert_color = curr_player == WHITE ? BLACK : WHITE;
							if (turn != CHECK_POS) game_on = 0;
							//alert_state(turn, alert_color); *** turn on when we have the func!
						}
						if (game_on) computer_turn(board, curr_player);
					}
					else{
						console_alert(turn);
						if (game_on) computer_turn(board, curr_player);
					}
					if (!game_on) break;

					curr_player = user_color; 
					turn = pre_turn_verify(board, user_color);
					if (gui_mode){
						Move * new_move = NULL;
						Move * move2do = NULL;
						while (move2do == NULL){
							new_move = gui_game_mode(turn, board); // why not to add the alerts here?
							move2do = is_valid_move(moves_head, new_move);
						}
						exc_move(board, move2do, curr_player);
					}
					else{
						console_alert(turn);
						if (game_on) user_turn(board, curr_player);
					}
					if (!game_on) break;
				}
			}
			// 2 players
			if (game_mode == 1){
				int turn;
				curr_player = start_color;
				turn = pre_turn_verify(board, curr_player);
				if (gui_mode){
					Move * new_move = NULL;
					Move * move2do = NULL;
					while (move2do == NULL){
						new_move = gui_game_mode(turn, board);
						move2do = is_valid_move(moves_head, new_move);
					}
					exc_move(board, move2do, curr_player);
				}
				else{
					console_alert(turn);
					if (game_on) user_turn(board, curr_player);
				}
				if (!game_on) break;

				curr_player = !start_color;
				turn = pre_turn_verify(board, curr_player);
				if (gui_mode){
					Move * new_move = NULL;
					Move * move2do = NULL;
					while (move2do == NULL){
						new_move = gui_game_mode(curr_player, board);
						move2do = is_valid_move(moves_head, new_move);
					}
					exc_move(board, move2do, curr_player);
				}
				else{
					console_alert(turn);
					if (game_on) user_turn(board, curr_player);
				}
				if (!game_on) break;
			}
		}
		char *command = input2str(stdin);
		free(command);
	}
}

	//if (end_pos == 1){
	//	free(command);
	//	command = input2str(stdin);
	//}
	//free(command);

	//main_sdl();
	//return 0;



