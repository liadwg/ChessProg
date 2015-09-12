//********************************* Draughts code ************************************//

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
	return (pos.col >= 0 && pos.col < BOARD_SIZE && pos.row >= 0 && pos.row < BOARD_SIZE && ((pos.col + pos.row) % 2 != 1));
}

int is_king(char piece){
	return (piece == WHITE_K || piece == BLACK_K);
}

int is_opposite(COLOR player, char piece){
	if (player == WHITE && (piece == BLACK_M || piece == BLACK_K)) return 1;
	if (player == BLACK && (piece == WHITE_M || piece == WHITE_K)) return 1;
	return 0;
}

// Helper func - checks if a piece reached the end of the board according to its color
int is_EOB(Pos piece, COLOR player){
	if (player == WHITE) return piece.row == BOARD_SIZE - 1;
	else return piece.row == 0;
}

// frees moves linked list
void clear_old_moves(Move* head){
	if (head != NULL){
		clear_old_moves(head->next);
		free(head->dest);
		free(head);
	}
}

// adds a move to the list, while making sure it contains valid moves only
void add_move(Pos piece, Pos* dests, int move_captures){
	if (moves == NULL){
		moves = malloc(sizeof(Move));
		moves_head = moves;
	}
	else if (moves->captures < move_captures  && abs(piece.row - dests->row) != 1){
		clear_old_moves(moves_head);
		moves = malloc(sizeof(Move));
		moves_head = moves;
	}
	else if ((moves->captures == 0 && abs(piece.row - dests->row) == 1) || (moves->captures == move_captures && abs(piece.row - dests->row) != 1)){
		moves->next = malloc(sizeof(Move));
		moves = moves->next;
	}
	else return;

	moves->piece.col = piece.col;
	moves->piece.row = piece.row;
	if (move_captures == 0){
		moves->dest = malloc(sizeof(Pos));
		moves->dest->row = dests->row;
		moves->dest->col = dests->col;
	}
	else{
		moves->dest = malloc(sizeof(Pos) * move_captures);
		for (int i = 0; i < move_captures; i++){
			moves->dest[i].row = dests[i].row;
			moves->dest[i].col = dests[i].col;
		}
	}

	moves->captures = move_captures;
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

// finds moves with chained captures
int get_capture_moves(Pos start, Pos piece, char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int count, Pos* dests){
	Pos pos[4] = { { piece.col - 1, piece.row - 1 }, { piece.col + 1, piece.row - 1 }, { piece.col - 1, piece.row + 1 }, { piece.col + 1, piece.row + 1 } };
	int found_now = 0, found_ahead = -1;
	for (int p = 0; p < 4; p++)
		if (is_valid_pos(pos[p]) && is_opposite(player, board[pos[p].col][pos[p].row])){
		Pos new_piece = get_next_diag(piece, pos[p]);
		if (is_valid_pos(new_piece) && board[new_piece.col][new_piece.row] == EMPTY){
			found_now++;
			char tmp = board[pos[p].col][pos[p].row];
			board[pos[p].col][pos[p].row] = EMPTY;
			Pos* new_dests = malloc(sizeof(Pos) * (count + 1));
			for (int i = 0; i < count; i++){
				new_dests[i].row = dests[i].row;
				new_dests[i].col = dests[i].col;
			}
			new_dests[count].row = new_piece.row;
			new_dests[count].col = new_piece.col;
			if (!(is_EOB(new_piece, player) && !is_king(curr_piece))) found_ahead = get_capture_moves(start, new_piece, board, player, count + 1, new_dests);
			if (found_ahead == 0 || (is_EOB(new_piece, player) && !is_king(curr_piece))){ add_move(start, new_dests, count + 1); }
			free(new_dests);
			board[pos[p].col][pos[p].row] = tmp;
		}
		}
	return found_now;
}

// finds all moves of a single man piece
void get_man_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos pos[4] = { { piece.col - 1, piece.row - 1 }, { piece.col + 1, piece.row - 1 }, { piece.col - 1, piece.row + 1 }, { piece.col + 1, piece.row + 1 } };
	int direction = 1, found_ahead = -1;
	if (player == BLACK) direction = -1;

	for (int p = 0; p < 4; p++){
		if (is_valid_pos(pos[p]) && pos[p].row == piece.row + direction && board[pos[p].col][pos[p].row] == EMPTY) add_move(piece, &pos[p], 0);
		else if (is_valid_pos(pos[p]) && is_opposite(player, board[pos[p].col][pos[p].row])){
			char tmp = board[pos[p].col][pos[p].row];
			board[pos[p].col][pos[p].row] = EMPTY;
			board[piece.col][piece.row] = EMPTY;
			Pos new_piece = get_next_diag(piece, pos[p]);
			if (is_valid_pos(new_piece) && board[new_piece.col][new_piece.row] == EMPTY){
				if (!is_EOB(new_piece, player)) found_ahead = get_capture_moves(piece, new_piece, board, player, 1, &new_piece);
				if (found_ahead == 0 || is_EOB(new_piece, player)) add_move(piece, &new_piece, 1);
			}
			board[pos[p].col][pos[p].row] = tmp;
			board[piece.col][piece.row] = curr_piece;
		}
	}
}

// finds all moves of a single king piece
void get_king_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos pos[4] = { { piece.col - 1, piece.row - 1 }, { piece.col + 1, piece.row - 1 }, { piece.col - 1, piece.row + 1 }, { piece.col + 1, piece.row + 1 } };
	Pos curr, new_piece;
	int found_ahead = -1;

	for (int p = 0; p < 4; p++){
		curr = pos[p];
		while (is_valid_pos(curr) && board[curr.col][curr.row] == EMPTY){
			add_move(piece, &curr, 0);
			curr = get_next_diag(piece, curr);
		}
		if (is_valid_pos(curr) && is_opposite(player, board[curr.col][curr.row])){
			char tmp = board[curr.col][curr.row];
			board[curr.col][curr.row] = EMPTY;
			board[piece.col][piece.row] = EMPTY;
			new_piece = get_next_diag(piece, curr);
			if (is_valid_pos(new_piece) && board[new_piece.col][new_piece.row] == EMPTY){
				found_ahead = get_capture_moves(piece, new_piece, board, player, 1, &new_piece);
				if (found_ahead == 0) add_move(piece, &new_piece, 1);
			}
			board[curr.col][curr.row] = tmp;
			board[piece.col][piece.row] = curr_piece;
		}
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
		curr_piece = board[i][j];
		if (is_king(curr_piece)) get_king_moves(board, player, p);
		else get_man_moves(board, player, p);
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