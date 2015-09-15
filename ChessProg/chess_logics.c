#include "ChessProg.h"

// Globals
Move* moves = NULL;
Move* moves_head = NULL;
char curr_piece;
COLOR curr_player;
Move* best_move;
COLOR user_color = WHITE;
int minimax_depth = 1;
int game_mode = 1; // 2 player mode


// Helper funcs
int is_valid_pos(Pos pos){
	//return (pos.col >= 0 && pos.col < BOARD_SIZE && pos.row >= 0 && pos.row < BOARD_SIZE && ((pos.col + pos.row) % 2 != 1));
	return (pos.col >= 0 && pos.col < BOARD_SIZE && pos.row >= 0 && pos.row < BOARD_SIZE);
}

int is_opposite(COLOR player, char piece){
	if (player == WHITE && (piece >= 'A' && piece <= 'Z')) return 1;
	if (player == BLACK && (piece >= 'a' && piece <= 'z')) return 1;
	return 0;
}

// Helper func - checks if a piece reached the end of the board according to its color
int is_EOB(Pos piece, COLOR player){
	if (player == WHITE) return piece.row == BOARD_SIZE - 1;
	else return piece.row == 0;
}

char* get_piece_name_by_type(int type){
	switch (type){
	case 1:
		return "queen";
	case 2:
		return "bishop";
	case 3:
		return "rook";
	case 4:
		return "knight";
	}
}

char get_piece_by_type(int type, COLOR player){
	switch (type){
	case 1:
		if (player == WHITE) return WHITE_Q;
		return BLACK_Q;
	case 2:
		if (player == WHITE) return WHITE_B;
		return BLACK_B;
	case 3:
		if (player == WHITE) return WHITE_R;
		return BLACK_R;
	case 4:
		if (player == WHITE) return WHITE_N;
		return BLACK_N;
	}
}

// frees moves linked list
void clear_old_moves(Move* head){
	if (head != NULL){
		clear_old_moves(head->next);
		//free(head->dest);
		free(head);
	}
}

// adds a move to the list
void add_move(Pos piece, Pos dest, int promote){
	if (moves == NULL){
		moves = malloc(sizeof(Move));
		moves_head = moves;
	}
	else{
		moves->next = malloc(sizeof(Move));
		moves = moves->next;
	}

	moves->piece.col = piece.col;
	moves->piece.row = piece.row;
	moves->dest.col = dest.col;
	moves->dest.row = dest.row;

	moves->promote = promote;
	moves->next = NULL;
}

// Helper funcs - return the next/prev pos in a diagonal
Pos get_next_diag(Pos from, Pos to){
	Pos res;
	if (from.col - to.col > 0) res.col = to.col - 1;
	else res.col = to.col + 1;
	if (from.row - to.row > 0) res.row = to.row - 1;
	else res.row = to.row + 1;
	return res;
}

Pos get_prev_diag(Pos from, Pos to){
	Pos res;
	if (from.col - to.col > 0) res.col = to.col + 1;
	else res.col = to.col - 1;
	if (from.row - to.row > 0) res.row = to.row + 1;
	else res.row = to.row - 1;
	return res;
}

void get_rook_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos curr = { piece.col, piece.row };
	while (is_valid_pos((Pos) { curr.col + 1, curr.row }) &&
		(board[curr.col + 1][curr.row] == EMPTY || is_opposite(player, board[curr.col + 1][curr.row]))){
		curr.col++; 
		add_move(piece, curr, 0);
	}
	curr.col = piece.col;
	//curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col - 1, curr.row }) &&
		(board[curr.col - 1][curr.row] == EMPTY || is_opposite(player, board[curr.col - 1][curr.row]))){
		curr.col--;
		add_move(piece, curr, 0);
	}
	curr.col = piece.col;
	//curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col, curr.row + 1 }) &&
		(board[curr.col][curr.row + 1] == EMPTY || is_opposite(player, board[curr.col][curr.row + 1]))){
		curr.row++;
		add_move(piece, curr, 0);
	}
	//curr.col = piece.col;
	curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col, curr.row - 1 }) &&
		(board[curr.col][curr.row - 1] == EMPTY || is_opposite(player, board[curr.col][curr.row - 1]))){
		curr.row--;
		add_move(piece, curr, 0);
	}
}

void get_bishop_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos curr = { piece.col, piece.row };
	while (is_valid_pos((Pos) { curr.col + 1, curr.row + 1 }) &&
		(board[curr.col + 1][curr.row + 1] == EMPTY || is_opposite(player, board[curr.col + 1][curr.row + 1]))){
		curr.col++;
		curr.row++;
		add_move(piece, curr, 0);
	}
	curr.col = piece.col;
	curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col - 1, curr.row - 1 }) &&
		(board[curr.col - 1][curr.row - 1] == EMPTY || is_opposite(player, board[curr.col - 1][curr.row - 1]))){
		curr.col--;
		curr.row--;
		add_move(piece, curr, 0);
	}
	curr.col = piece.col;
	curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col - 1, curr.row + 1 }) &&
		(board[curr.col - 1][curr.row + 1] == EMPTY || is_opposite(player, board[curr.col - 1][curr.row + 1]))){
		curr.col--;
		curr.row++;
		add_move(piece, curr, 0);
	}
	curr.col = piece.col;
	curr.row = piece.row;
	while (is_valid_pos((Pos) { curr.col + 1, curr.row - 1 }) &&
		(board[curr.col + 1][curr.row - 1] == EMPTY || is_opposite(player, board[curr.col + 1][curr.row - 1]))){
		curr.col++;
		curr.row--;
		add_move(piece, curr, 0);
	}
}

void get_pawn_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	int direction = board[piece.col][piece.row] == BLACK_P ? -1 : 1;
	int pawn_row = board[piece.col][piece.row] == BLACK_P ? 6 : 1;
	Pos dest;

	if (board[piece.col][piece.row + direction] == EMPTY){
		dest.col = piece.col;
		dest.row = piece.row + direction;
		add_move(piece, dest, 0);
		if (is_EOB(dest, player))
			for (int i = 1; i < 5; i++) add_move(piece, dest, i);
	}
	if (piece.row == pawn_row && board[piece.col][piece.row + (2 * direction)] == EMPTY){
		dest.col = piece.col;
		dest.row = piece.row + (2 * direction);
		add_move(piece, dest, 0);
	}
	if (is_valid_pos((Pos) { piece.col + 1, piece.row + direction }) && is_opposite(player, board[piece.col + 1][piece.row + direction])){
		dest.col = piece.col + 1;
		dest.row = piece.row + direction;
		add_move(piece, dest, 0);
		if (is_EOB(dest, player))
			for (int i = 1; i < 5; i++) add_move(piece, dest, i);
	}
	if (is_valid_pos((Pos) { piece.col - 1, piece.row + direction }) && is_opposite(player, board[piece.col + 1][piece.row + direction])){
		dest.col = piece.col - 1;
		dest.row = piece.row + direction;
		add_move(piece, dest, 0);
		if (is_EOB(dest, player))
			for (int i = 1; i < 5; i++) add_move(piece, dest, i);
	}
}

void get_moves_by_piece(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos* tmp_dests;
	int dests_num = 0;
	switch (board[piece.col][piece.row]){
	case WHITE_B:
	case BLACK_B:
		return get_bishop_moves(board, player, piece);
	case WHITE_R:
	case BLACK_R:
		return get_rook_moves(board, player, piece);
	case WHITE_Q:
	case BLACK_Q:
		get_bishop_moves(board, player, piece);
		return get_rook_moves(board, player, piece);
	case BLACK_P:
	case WHITE_P:
		return get_pawn_moves;
	case WHITE_K:
	case BLACK_K:
		tmp_dests = malloc(sizeof(Pos) * 8);
		tmp_dests = (Pos[8]) { { piece.col - 1, piece.row - 1 }, { piece.col - 1, piece.row }, { piece.col - 1, piece.row + 1 },
							   { piece.col + 1, piece.row - 1 }, { piece.col + 1, piece.row }, { piece.col + 1, piece.row + 1 },
							   { piece.col, piece.row - 1 }, { piece.col, piece.row + 1 } };
		break;
	case WHITE_N:
	case BLACK_N:
		tmp_dests = malloc(sizeof(Pos) * 8);
		tmp_dests = (Pos[8]) { { piece.col - 2, piece.row - 1 }, { piece.col - 2, piece.row + 1 },
							   { piece.col + 2, piece.row - 1 }, { piece.col + 2, piece.row + 1 },
							   { piece.col - 1, piece.row - 2 }, { piece.col + 1, piece.row - 2 },
							   { piece.col - 1, piece.row + 2 }, { piece.col + 1, piece.row + 2 } };
		break;
	default:
		return;
	
	for (int i = 0; i < 8; i++)
		if (is_valid_pos(tmp_dests[i]) && (board[tmp_dests[i].col][tmp_dests[i].row] == EMPTY || is_opposite(player, board[tmp_dests[i].col][tmp_dests[i].row]))) 
			add_move(piece, tmp_dests[i], 0);
	
	free(tmp_dests);
	return;
	}
}

Move * get_all_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	moves_head = NULL;
	moves = NULL;
	Pos p;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (!is_opposite(player, board[i][j]) && board[i][j] != EMPTY){
				p.row = j;
				p.col = i;
				get_moves_by_piece(board, player, p);
			}
	return moves_head;
}

// prints a single move in a specific format
void print_move(Move* head){
	printf("<%c,%d> to <%c,%d>", head->piece.col + 97, head->piece.row + 1, head->dest[0].col + 97, head->dest[0].row + 1);
	for (int i = 1; i < head->captures; i++){
		printf("<%c,%d>", head->dest[i].col + 97, head->dest[i].row + 1);
	}
	printf("\n");
}

void print_moves(Move* head){
	while (head != NULL){
		print_move(head);
		head = head->next;
	}
}

// calc_score helper func
int get_piece_score(char piece, COLOR player){
	if (player == WHITE) switch (piece){
	case BLACK_M: return -1;
	case BLACK_K: return -3;
	case WHITE_M: return 1;
	case WHITE_K: return 3;
	}
	else switch (piece){
	case BLACK_M: return 1;
	case BLACK_K: return 3;
	case WHITE_M: return -1;
	case WHITE_K: return -3;
	}
	return 0;
}

// calculates the score of the board from a player's prospective
int calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	int score = 0, opposites = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++){
		if (is_opposite(player, board[i][j])) opposites++;
		score += get_piece_score(board[i][j], player);
		}
	if (opposites == 0) return 100;
	return score;
}

// Helper func
void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]){
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) board2[i][j] = board1[i][j];
}

// minimax recursive func, using alpha-beta pruning
int alpha_beta_minimax(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int depth, int alpha, int beta){
	Move* move_list = get_all_moves(board, player);
	Move* curr_move = move_list;
	if (depth == 0 && curr_move == NULL){
		best_move = NULL;
		return -100;
	}
	if (curr_move == NULL && player != curr_player) return 100;
	if (curr_move == NULL && player == curr_player) return -100;
	if (depth == minimax_depth || curr_move == NULL){
		clear_old_moves(move_list);
		return calc_score(board, curr_player);
	}

	if (depth == 0){
		best_move = curr_move;
		if (curr_move->next == NULL) return 100;
	}

	int tmp;
	char init_board[BOARD_SIZE][BOARD_SIZE];
	duplicate_board(board, init_board);
	// MAX
	if (depth % 2 == 0){
		while (curr_move != NULL){
			exc_move(board, curr_move);
			tmp = alpha_beta_minimax(board, (player == 0), depth + 1, alpha, beta);
			if (tmp > alpha){
				alpha = tmp;
				if (depth == 0) best_move = curr_move;
			}
			if (alpha >= beta){
				if (depth != 0) clear_old_moves(move_list);
				else moves_head = move_list;
				duplicate_board(init_board, board);
				return alpha;
			}
			curr_move = curr_move->next;
			duplicate_board(init_board, board);
		}
		if (depth != 0) clear_old_moves(move_list);
		else moves_head = move_list;
		return alpha;
	}
	// MIN
	else{
		while (curr_move != NULL){
			exc_move(board, curr_move);
			tmp = alpha_beta_minimax(board, (player == 0), depth + 1, alpha, beta);
			if (tmp < beta){
				beta = tmp;
				if (depth == 0) best_move = curr_move;
			}
			if (alpha >= beta){
				if (depth != 0) clear_old_moves(move_list);
				else moves_head = move_list;
				duplicate_board(init_board, board);
				return beta;
			}
			curr_move = curr_move->next;
			duplicate_board(init_board, board);
		}
		if (depth != 0) clear_old_moves(move_list);
		else moves_head = move_list;
		return beta;
	}
}

// safety check before starting the game
int is_valid_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int b_num = 0;
	int w_num = 0;
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if (board[i][j] == BLACK_M || board[i][j] == BLACK_K) b_num++;
			if (board[i][j] == WHITE_M || board[i][j] == WHITE_K) w_num++;
		}
	}
	if (b_num == 0 || w_num == 0 || b_num > 20 || w_num > 20) return 0;
	return 1;
}

// Helper func - checks if a piece belongs to the player
int is_valid_piece(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color){
	if (color == BLACK){
		if (board[move->piece.col][move->piece.row] == BLACK_M || board[move->piece.col][move->piece.row] == BLACK_K) return 1;
	}
	if (color == WHITE) {
		if (board[move->piece.col][move->piece.row] == WHITE_M || board[move->piece.col][move->piece.row] == WHITE_K) return 1;
	}
	return 0;
}

// checks if a move is in the valid moves list
Move * is_valid_move(Move * moves, Move * new_move){
	Move * current_move = moves;
	while (current_move != NULL){
		if (current_move->piece.col == new_move->piece.col &&
			current_move->piece.row == new_move->piece.row &&
			(current_move->captures == new_move->captures || (current_move->captures == 0 && new_move->captures == 1))){
			for (int i = 0; i < new_move->captures; i++){
				if (current_move->dest[i].col != new_move->dest[i].col || current_move->dest[i].row != new_move->dest[i].row) break;
				if (i == new_move->captures - 1) return current_move;
			}
		}
		current_move = current_move->next;
	}
	return NULL;
}