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
		printf((j < 9 ? " %d" : "%d"), j + 1);
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

void init_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (j <= 3){
					board[i][j] = WHITE_M;
				}
				else if (j >= 6){
					board[i][j] = BLACK_M;
				}
				else{
					board[i][j] = EMPTY;
				}
			}
			else{
				board[i][j] = EMPTY;
			}
		}
	}
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
	if (strcmp(word1, "minimax_depth") == 0){
		int x = atoi(strtok(NULL, " "));
		if (x > 6 || x < 1) printf(WRONG_MINIMAX_DEPTH);
		else minimax_depth = x;
	}
	else if (strcmp(word1, "user_color") == 0){
		char * color = strtok(NULL, " ");
		if (strcmp(color, "black") == 0) user_color = BLACK;
	}
	else if (strcmp(word1, "clear") == 0) clear_board(board);
	else if (strcmp(word1, "rm") == 0){
		char * coor1 = strtok(NULL, " <,>");
		char * coor2 = strtok(NULL, " <,>");
		if (coor1[0] < 'a' || coor1[0] > 'j' || atoi(coor2) < 1 ||
			atoi(coor2) > 10 || (coor1[0] - 'a' + atoi(coor2) - 1) % 2 == 1) {
			printf(WRONG_POSITION);
		}
		else board[coor1[0] - 'a'][atoi(coor2) - 1] = EMPTY;
	}
	else if (strcmp(word1, "set") == 0){
		char * coor1 = strtok(NULL, " <,>");
		char * coor2 = strtok(NULL, " <,>");
		if (coor1[0] < 'a' || coor1[0] > 'j' || atoi(coor2) < 1 ||
			atoi(coor2) > 10 || (coor1[0] - 'a' + atoi(coor2) - 1) % 2 == 1){
			printf(WRONG_POSITION);
			return;
		}
		char * a = strtok(NULL, " ");
		if (a == NULL) return;
		char * b = strtok(NULL, " ");
		if (strcmp(a, "black") == 0){
			if (strcmp(b, "m") == 0) board[coor1[0] - 'a'][atoi(coor2) - 1] = BLACK_M;
			else  board[coor1[0] - 'a'][atoi(coor2) - 1] = BLACK_K;
		}
		else {
			if (strcmp(b, "m") == 0) board[coor1[0] - 'a'][atoi(coor2) - 1] = WHITE_M;
			else  board[coor1[0] - 'a'][atoi(coor2) - 1] = WHITE_K;
		}
	}
	else if (strcmp(word1, "print") == 0) print_board(board);
	else printf(ILLEGAL_COMMAND);
	return;
}

// manages the computer's turn
int computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color){
	curr_player = color;
	alpha_beta_minimax(board, color, 0, -100, 100);
	Move * move2do = best_move;
	int ret_val;
	if (move2do == NULL) ret_val = WIN_POS;
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
	if (moves_head == NULL) return WIN_POS;
	char *word1;
	char *command = NULL;
	Move* new_move = NULL;
	int ret_val;
	while (1){
		printf(ENTER_YOUR_MOVE);
		if (new_move != NULL) clear_old_moves(new_move);
		new_move = malloc(sizeof(Move));
		new_move->dest = malloc(sizeof(Pos) * 2 * BOARD_SIZE);
		new_move->next = NULL;
		if (command != NULL) free(command);
		command = input2str(stdin);
		word1 = strtok(command, " ");
		if (strcmp(word1, "quit") == 0){
			ret_val = QUIT;
			break;
		}
		else if (strcmp(word1, "get_moves") == 0){
			print_moves(moves_head);
			continue;
		}
		else if (strcmp(word1, "move") == 0){
			char * piece_coor1 = strtok(NULL, " <,>");
			char * piece_coor2 = strtok(NULL, " <,>");
			new_move->piece.col = piece_coor1[0] - 'a';
			new_move->piece.row = atoi(piece_coor2) - 1;
			if (!is_valid_pos(new_move->piece)){
				printf(WRONG_POSITION);
				continue;
			}
			int i = 0;
			char * dest_coor1 = strtok(NULL, " <,>to");
			char * dest_coor2 = strtok(NULL, " <,>to");
			while (dest_coor1 != NULL){
				new_move->dest[i].col = dest_coor1[0] - 'a';
				new_move->dest[i].row = atoi(dest_coor2) - 1;
				if (!is_valid_pos(new_move->dest[i])){
					i = -1;
					break;
				}
				i++;
				dest_coor1 = strtok(NULL, " <,>[]");
				if (dest_coor1 != NULL) dest_coor2 = strtok(NULL, " <,>[]");
			}
			if (i == -1){
				printf(WRONG_POSITION);
				continue;
			}
			if (!is_valid_piece(board, new_move, color)){
				printf(NO_DISC);
				continue;
			}
			new_move->dest = realloc(new_move->dest, sizeof(Pos) * i);
			new_move->captures = i;
			Move * move2do = is_valid_move(moves_head, new_move);
			if (move2do == NULL){
				printf(ILLEGAL_MOVE);
				continue;
			}
			else{
				exc_move(board, move2do);
				print_board(board);
				ret_val = GAME_ON;
				break;
			}
		}
		else printf(ILLEGAL_COMMAND);
	}
	free(command);
	clear_old_moves(new_move);
	clear_old_moves(moves_head);
	return ret_val;
}

// executes a specific move on the given board
void exc_move(char board[BOARD_SIZE][BOARD_SIZE], Move * move){
	Pos cur, cap;
	cur.col = move->piece.col;
	cur.row = move->piece.row;
	int dest_len = move->captures;
	if (dest_len == 0) dest_len = 1;
	for (int i = 0; i < dest_len; i++){
		board[move->dest[i].col][move->dest[i].row] = board[cur.col][cur.row];
		if (board[cur.col][cur.row] == BLACK_M && move->dest[i].row == 0) board[move->dest[i].col][move->dest[i].row] = BLACK_K;
		if (board[cur.col][cur.row] == WHITE_M && move->dest[i].row == BOARD_SIZE - 1) board[move->dest[i].col][move->dest[i].row] = WHITE_K;
		board[cur.col][cur.row] = EMPTY;
		if (move->captures > 0){
			cap = get_prev_diag(cur, move->dest[i]);
			board[cap.col][cap.row] = EMPTY;
		}
		cur.col = move->dest[i].col;
		cur.row = move->dest[i].row;
	}
}

int main(void)
{
	char board[BOARD_SIZE][BOARD_SIZE];
	init_board(board);
	printf(WELCOME_TO_DRAUGHTS);
	printf(ENTER_SETTINGS);
	char *command = input2str(stdin);
	int win_pos = 0;

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
			if (user_color == WHITE){
				int ret_val = user_turn(board, WHITE);
				if (ret_val == QUIT) break;
				if (ret_val == WIN_POS){
					printf(BLACK_WIN);
					win_pos = 1;
					break;
				}
				if (computer_turn(board, BLACK) == WIN_POS){
					printf(WHITE_WIN);
					win_pos = 1;
					break;
				}
			}
			else{
				if (computer_turn(board, WHITE) == WIN_POS){
					printf(BLACK_WIN);
					win_pos = 1;
					break;
				}
				int ret_val = user_turn(board, BLACK);
				if (ret_val == QUIT) break;
				if (ret_val == WIN_POS){
					printf(WHITE_WIN);
					win_pos = 1;
					break;
				}
			}
		}
	}
	if (win_pos == 1){
		free(command);
		command = input2str(stdin);
	}
	free(command);
}


