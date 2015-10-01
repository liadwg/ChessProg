#include "ChessProg.h"
#include "chess_utils.c"

// Globals
Move* moves = NULL;
Move* moves_head = NULL;
char curr_piece;
COLOR curr_player;
Move* best_move;
COLOR user_color = WHITE;
COLOR next_player = WHITE; // recently added 
int minimax_depth = 1;
int game_mode = 1; // 2 player mode
int best_depth = 0;
int board_count = 0;
char black_promotables[4] = { BLACK_B, BLACK_N, BLACK_Q, BLACK_R };
char white_promotables[4] = { WHITE_B, WHITE_N, WHITE_Q, WHITE_R };


// Helper funcs
int is_valid_pos(Pos pos){
	return (pos.col >= 0 && pos.col < BOARD_SIZE && pos.row >= 0 && pos.row < BOARD_SIZE);
}

int is_opposite(COLOR player, char piece){
	if (player == WHITE && (piece >= 'A' && piece <= 'Z')) return 1;
	if (player == WHITE && (piece >= 'a' && piece <= 'z')) return 1;
	return 0;
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
	case 5:
		if (player == WHITE) return WHITE_P;
		return BLACK_P;
	case 6:
		if (player == WHITE) return WHITE_K;
		return BLACK_K;
	}
}

// Helper func - checks if a piece belongs to the player
int is_valid_piece(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color){
	if (is_valid_pos(move->piece) && board[move->piece.col][move->piece.row] != EMPTY)
		return !is_opposite(color, board[move->piece.col][move->piece.row]);
	return 0;
}

// Helper func - checks if a piece reached the end of the board according to its color
int is_EOB(Pos piece, COLOR player){
	if (player == WHITE) return piece.row == BOARD_SIZE - 1;
	else return piece.row == 0;
}

// Helper func
void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]){
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) board2[i][j] = board1[i][j];
	}

int is_pos_threatened(Pos pos, char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	int dir = player == WHITE ? -1 : 1;
	Pos curr = { pos.col, pos.row };
	int blocked[4] = { 0 };
	char* pieces;
	char bpieces[6] = { BLACK_P, BLACK_B, BLACK_N, BLACK_R, BLACK_Q, BLACK_K };
	char wpieces[6] = { WHITE_P, WHITE_B, WHITE_N, WHITE_R, WHITE_Q, WHITE_K };
	if (player == WHITE) pieces = bpieces;
	else pieces = wpieces;

	// check pawns
	if (board[pos.col - 1][pos.row + dir] == pieces[0] || board[pos.col + 1][pos.row + dir] == pieces[0]) return 1;

	// check bishops + queen
	int i = 1;
	while (i < BOARD_SIZE){
		if (is_valid_pos((Pos) { pos.col - i, pos.row - i }) && !blocked[0])
			if (board[pos.col - i][pos.row - i] == pieces[1] || board[pos.col - i][pos.row - i] == pieces[4]) return 1;
			else{ if (board[pos.col - i][pos.row - i] != EMPTY) blocked[0] = 1; }
		if (is_valid_pos((Pos) { pos.col + i, pos.row - i }) && !blocked[1])
			if (board[pos.col + i][pos.row - i] == pieces[1] || board[pos.col + i][pos.row - i] == pieces[4]) return 1;
			else{ if (board[pos.col + i][pos.row - i] != EMPTY) blocked[1] = 1; }
		if (is_valid_pos((Pos) { pos.col - i, pos.row + i }) && !blocked[2])
			if (board[pos.col - i][pos.row + i] == pieces[1] || board[pos.col - i][pos.row + i] == pieces[4]) return 1;
			else{ if (board[pos.col - i][pos.row + i] != EMPTY) blocked[2] = 1; }
		if (is_valid_pos((Pos) { pos.col + i, pos.row + i }) && !blocked[3])
			if (board[pos.col + i][pos.row + i] == pieces[1] || board[pos.col + i][pos.row + i] == pieces[4]) return 1;
			else{ if (board[pos.col + i][pos.row + i] != EMPTY) blocked[3] = 1; }
		i++;
}

	// check knights
	Pos dests[8] = {{ pos.col - 2, pos.row - 1 }, { pos.col - 2, pos.row + 1 },
					{ pos.col + 2, pos.row - 1 }, { pos.col + 2, pos.row + 1 },
					{ pos.col - 1, pos.row - 2 }, { pos.col + 1, pos.row - 2 },
					{ pos.col - 1, pos.row + 2 }, { pos.col + 1, pos.row + 2 }};
	foreach(Pos *p, dests){
		if (is_valid_pos(*p))
			if (board[p->col][p->row] == pieces[2]) return 1;
	}

	// check rooks + queen
	i = 1;
	for (int j = 0; j < 4; j++) blocked[j] = 0;
	while (i < BOARD_SIZE){
		if (is_valid_pos((Pos) { pos.col - i, pos.row }) && !blocked[0])
			if (board[pos.col - i][pos.row] == pieces[3] || board[pos.col - i][pos.row] == pieces[4]) return 1;
			else{ if (board[pos.col - i][pos.row] != EMPTY) blocked[0] = 1; }
		if (is_valid_pos((Pos) { pos.col + i, pos.row }) && !blocked[1])
			if (board[pos.col + i][pos.row] == pieces[3] || board[pos.col + i][pos.row] == pieces[4]) return 1;
			else{ if (board[pos.col + i][pos.row] != EMPTY) blocked[1] = 1; }
		if (is_valid_pos((Pos) { pos.col, pos.row - i }) && !blocked[2])
			if (board[pos.col][pos.row - i] == pieces[3] || board[pos.col][pos.row - i] == pieces[4]) return 1;
			else{ if (board[pos.col][pos.row - i] != EMPTY) blocked[2] = 1; }
		if (is_valid_pos((Pos) { pos.col, pos.row + i }) && !blocked[3])
			if (board[pos.col][pos.row + i] == pieces[3] || board[pos.col][pos.row + i] == pieces[4]) return 1;
			else{ if (board[pos.col][pos.row + i] != EMPTY) blocked[3] = 1; }
		i++;
	}

	// check king
	Pos kdests[8] = {{ pos.col - 1, pos.row - 1 }, { pos.col - 1, pos.row }, { pos.col - 1, pos.row + 1 },
					 { pos.col + 1, pos.row - 1 }, { pos.col + 1, pos.row }, { pos.col + 1, pos.row + 1 },
					 { pos.col, pos.row - 1 }, { pos.col, pos.row + 1 }};
	foreach(Pos *p, kdests){
		if (is_valid_pos(*p))
			if (board[p->col][p->row] == pieces[5]) return 1;
	}

	// no threat
	return 0;
}

int is_check(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	char king = player == WHITE ? WHITE_K : BLACK_K;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (board[i][j] == king){
		return is_pos_threatened((Pos) { i, j }, board, !player);
		//if (!is_pos_threatened((Pos) { i, j }, board, !player)) return 0;

		//Pos kdests[8] = {{ i - 1, j - 1 }, { i - 1, j }, { i - 1, j + 1 },
		//				{ i + 1, j - 1 }, { i + 1, j }, { i + 1, j + 1 },
		//				{ i, j - 1 }, { i, j + 1 }};
		//foreach(Pos *p, kdests){
		//	if (!is_pos_threatened(*p, board, !player)) return 1;
		//} 
		//// king is threatned and has nowhere to go, return mate
		//return 2;
			}
}


int piece_counter(char board[BOARD_SIZE][BOARD_SIZE], int * whites, int * blacks){
	int white_b = -1, black_b = -1, bishop_fault = 0;

	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			switch (board[i][j]){
			case WHITE_P:
				whites[0]++;
				break;
			case WHITE_B:
				whites[1]++;
				if (white_b != -1 && (i + j) % 2 == white_b) bishop_fault = 1;
				white_b = (i + j) % 2;
				break;
			case WHITE_N:
				whites[2]++;
				break;
			case WHITE_R:
				whites[3]++;
				break;
			case WHITE_Q:
				whites[4]++;
				break;
			case WHITE_K:
				whites[5]++;
				break;
			case BLACK_P:
				blacks[0]++;
				break;
			case BLACK_B:
				blacks[1]++;
				if (black_b != -1 && (i + j) % 2 == black_b) bishop_fault = 1;
				black_b = (i + j) % 2;
				break;
			case BLACK_N:
				blacks[2]++;
				break;
			case BLACK_R:
				blacks[3]++;
				break;
			case BLACK_Q:
				blacks[4]++;
				break;
			case BLACK_K:
				blacks[5]++;
				break;
			default:
				break;
			}
		}
	}
	return bishop_fault;
}

int clear_illegal_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	Move *prev_move = moves_head, *curr_move = moves_head;
	char tmp_board[BOARD_SIZE][BOARD_SIZE];
	while (curr_move != NULL){
		duplicate_board(board, tmp_board);
		exc_move(tmp_board, curr_move);
		if (is_check(tmp_board, player) != 0){
			if (curr_move == moves_head){
				moves_head = curr_move->next;
				prev_move = moves_head;
			}
			else prev_move->next = curr_move->next;
			free(curr_move);
		}
		else prev_move = curr_move;
		if (prev_move != NULL) curr_move = prev_move->next;
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
void add_move(Pos piece, Pos dest, char promote){
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
	char *promotables = player == WHITE ? white_promotables : black_promotables;

	Pos dest;

	if (board[piece.col][piece.row + direction] == EMPTY){
		dest.col = piece.col;
		dest.row = piece.row + direction;
		add_move(piece, dest, 0);
		if (is_EOB(dest, player))
			foreach(char *c, promotables) add_move(piece, dest, c);
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
			foreach(char *c, promotables) add_move(piece, dest, c);
	}
	if (is_valid_pos((Pos) { piece.col - 1, piece.row + direction }) && is_opposite(player, board[piece.col + 1][piece.row + direction])){
		dest.col = piece.col - 1;
		dest.row = piece.row + direction;
		add_move(piece, dest, 0);
		if (is_EOB(dest, player))
			foreach(char *c, promotables) add_move(piece, dest, c);
	}
}

void get_moves_by_piece(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos* tmp_dests;
	int dests_num = 0;
	switch (board[piece.col][piece.row]){
	case WHITE_B:
	case BLACK_B:
		get_bishop_moves(board, player, piece);
		return;
	case WHITE_R:
	case BLACK_R:
		get_rook_moves(board, player, piece);
		return;
	case WHITE_Q:
	case BLACK_Q:
		get_bishop_moves(board, player, piece);
		get_rook_moves(board, player, piece);
		return;
	case BLACK_P:
	case WHITE_P:
		get_pawn_moves(board, player, piece);
		return;
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
	clear_illegal_moves(board, player);
	return moves_head;
}

// prints a single move in a specific format
void print_move(Move* move){
	printf("<%c,%d> to <%c,%d>", move->piece.col + 97, move->piece.row + 1, move->dest.col + 97, move->dest.row + 1);
	if (move->promote > 0) printf(" %s", get_piece_name_by_type);
	printf("\n");
}


void print_moves(Move* head){
	while (head != NULL){
		print_move(head);
		head = head->next;
	}
}

void print_piece_moves(Move* head, int col, int row){
	while (head != NULL){
		if (head->piece.col == col && head->piece.col == row){
		print_move(head);
		}
		head = head->next;
	}
}

void print_best_moves(Move* head, int score){
	while (head != NULL){
		if (head->score == score){
			print_move(head);
		}
		head = head->next;
	}
}

// calculates the score of the board from a player's prospective
int calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	int whites[6] = { 0 }, blacks[6] = { 0 };

	piece_counter(board, &whites, &blacks);

	int white_score = whites[0] + 3 * whites[1] + 3 * whites[2] + 5 * whites[3] + 9 * whites[4] + 400 * whites[5];
	int black_score = blacks[0] + 3 * blacks[1] + 3 * blacks[2] + 5 * blacks[3] + 9 * blacks[4] + 400 * blacks[5];
	if (best_depth) board_count++;
	if (player == WHITE) return white_score - black_score;
	else return black_score - white_score;
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
	if (depth == minimax_depth || curr_move == NULL || (best_depth && board_count == BOARD_LIMIT)){
		clear_old_moves(move_list);
		return calc_score(board, curr_player);
	}

	if (depth == 0){
		best_move = curr_move;
		best_move->score = 100;
		if (curr_move->next == NULL) return 100;
	}

	char init_board[BOARD_SIZE][BOARD_SIZE];
	duplicate_board(board, init_board);
	// MAX
	if (depth % 2 == 0){
		while (curr_move != NULL){
			exc_move(board, curr_move);
			curr_move->score = alpha_beta_minimax(board, (player == 0), depth + 1, alpha, beta);
			if (curr_move->score > alpha){
				alpha = curr_move->score;
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
			curr_move->score = alpha_beta_minimax(board, (player == 0), depth + 1, alpha, beta);
			if (curr_move->score < beta){
				beta = curr_move->score;
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
	int whites[6] = { 0 }, blacks[6] = { 0 };
	int bishop_fault = 0;
	
	bishop_fault = piece_counter(board, &whites, &blacks);

	if (whites[0] > 8 || whites[5] != 1 || whites[4] > 1 || blacks[0] > 8 || blacks[5] != 1 || blacks[4] > 1 || bishop_fault) return 0;
	for (int i = 1; i < 4; i++) if (whites[i] > 2 || blacks[i] > 2) return 0;
	return 1;
}

// checks if a move is in the valid moves list
Move * is_valid_move(Move * moves, Move * new_move){
	Move * current_move = moves;
	while (current_move != NULL){
		if (current_move->piece.col == new_move->piece.col &&
			current_move->piece.row == new_move->piece.row &&
			current_move->promote == new_move->promote)
			return current_move;
		
		current_move = current_move->next;
	}
	return NULL;
}