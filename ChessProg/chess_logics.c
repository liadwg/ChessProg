#include "chess_logics.h"

// Globals
Move* moves = NULL;
Move* moves_head = NULL;
char curr_piece;
COLOR curr_player;
Move* best_move;
COLOR user_color = WHITE; 
COLOR start_color = WHITE;
int minimax_depth = 1;
int game_mode = 1; // player vs player mode (2 -> player vs AI)
int best_depth = 0;
int board_count = 0;
int gui_mode = 0;
int game_on = 1;
char black_promotables[4] = { BLACK_B, BLACK_N, BLACK_Q, BLACK_R };
char white_promotables[4] = { WHITE_B, WHITE_N, WHITE_Q, WHITE_R };


// Helper funcs
int is_valid_pos(Pos pos){
	return (pos.col >= 0 && pos.col < BOARD_SIZE && pos.row >= 0 && pos.row < BOARD_SIZE);
}

int is_opposite(COLOR player, char piece){
	if (player == WHITE && (piece >= 'A' && piece <= 'Z')) return 1;
	if (player == BLACK && (piece >= 'a' && piece <= 'z')) return 1;
	return 0;
}

char* get_piece_name_by_type(int type){
	switch (type){
	case 0: return "king";// may  cause some probs, was case 6
	case 1: return "queen";
	case 2: return "bishop";
	case 3: return "rook";
	case 4: return "knight";
	case 5: return "pawn";
	//default: return;
	}
	return NULL;
}

int get_type_by_name(char * piece){
	if (piece == NULL) return 0;
	if (strcmp(piece, "queen") == 0) return 1;
	if (strcmp(piece, "bishop") == 0) return 2;
	if (strcmp(piece, "rook") == 0) return 3;
	if (strcmp(piece, "knight") == 0) return 4;
	return 0;
}

char get_piece_by_name(char * name, COLOR color){
	char piece;
	if (strcmp(name, "king") == 0) piece = 'k';
	if (strcmp(name, "queen") == 0) piece = 'q';
	if (strcmp(name, "rook") == 0) piece = 'r';
	if (strcmp(name, "knight") == 0) piece = 'n';
	if (strcmp(name, "bishop") == 0) piece = 'b';
	if (strcmp(name, "pawn") == 0) piece = 'm';
	if (color == BLACK) piece = toupper(piece);
	return piece;
}


char get_piece_by_type(int type, COLOR player){
	switch (type){
	case 0: // may  cause some probs, was case 6
		if (player == WHITE) return WHITE_K;
		return BLACK_K;
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
	}
	return '\0';
}

int get_type_by_piece(char piece){
	switch (piece){
	case WHITE_K: case BLACK_K: return 0;
	case WHITE_Q: case BLACK_Q: return 1;
	case WHITE_B: case BLACK_B: return 2;
	case WHITE_R: case BLACK_R: return 3;
	case WHITE_N: case BLACK_N: return 4;
	case WHITE_P: case BLACK_P: return 5;
	}
}

COLOR get_color_by_piece(char piece){
	if (piece == WHITE_K || piece == WHITE_Q || piece == WHITE_B || piece == WHITE_R || piece == WHITE_N || piece == WHITE_P) return WHITE;
	else return BLACK;
}

// Helper func - checks if a piece belongs to the player
int is_valid_piece(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color){
	if (is_valid_pos(move->piece) && board[move->piece.col][move->piece.row] != EMPTY)
		return !is_opposite(color, board[move->piece.col][move->piece.row]);
	return 0;
}

// Helper func - checks if a piece reached the end of the board according to its color
int is_EOB(Pos piece, COLOR player){
	if (player == BLACK) return piece.row == 0;
	else return piece.row == BOARD_SIZE - 1;
}

// Helper func
void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]){
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) board2[i][j] = board1[i][j];
	}

int is_pos_threatened(Pos pos, char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	Pos curr; 
	//curr.col = pos.col; 
	//curr.row = pos.row;
	int blocked[4] = { 0 };
	char* pieces;
	char bpieces[6] = { BLACK_P, BLACK_B, BLACK_N, BLACK_R, BLACK_Q, BLACK_K };
	char wpieces[6] = { WHITE_P, WHITE_B, WHITE_N, WHITE_R, WHITE_Q, WHITE_K };
	pieces = (player == WHITE) ? bpieces : wpieces;

	// check pawns
	int dir = player == WHITE ? 1 : -1;
	curr.row = pos.row + dir;
	curr.col = pos.col - 1;
	if (is_valid_pos(curr)){
		if (board[curr.col][curr.row] == pieces[0]) return 1;
	}
	curr.col = pos.col + 1;
	if (is_valid_pos(curr)){
		if (board[curr.col][curr.row] == pieces[0]) return 1;
	}

	// check bishops + queen
	for (int i = 1; i < BOARD_SIZE; i++){
		curr.col = pos.col - i, curr.row = pos.row - i;
		if (is_valid_pos(curr) && !blocked[0]){
			blocked[0] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[1] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.col = pos.col + i;
		if (is_valid_pos(curr) && !blocked[1]){
			blocked[1] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[1] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.row = pos.row + i;
		if (is_valid_pos(curr) && !blocked[2]){
			blocked[2] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[1] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.col = pos.col - i;
		if (is_valid_pos(curr) && !blocked[3]){
			blocked[3] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[1] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
	}


	// check knights
	Pos dests[8] = {{ pos.col - 2, pos.row - 1 }, { pos.col - 2, pos.row + 1 },
					{ pos.col + 2, pos.row - 1 }, { pos.col + 2, pos.row + 1 },
					{ pos.col - 1, pos.row - 2 }, { pos.col + 1, pos.row - 2 },
					{ pos.col - 1, pos.row + 2 }, { pos.col + 1, pos.row + 2 }};
	for (int i = 0; i < 8; i++){
		if (is_valid_pos(dests[i])){
			if (board[dests[i].col][dests[i].row] == pieces[2]) return 1;
		}
	}


	// check rooks + queen
	for (int j = 0; j < 4; j++) blocked[j] = 0;
	for (int i = 1; i < BOARD_SIZE; i++){
		curr.col = pos.col - i; curr.row = pos.row;
		if (is_valid_pos(curr) && !blocked[0]){
			blocked[0] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[3] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.col = pos.col + i;
		if (is_valid_pos(curr) && !blocked[1]){
			blocked[1] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[3] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.col = pos.col, curr.row = pos.row - i;
		if (is_valid_pos(curr) && !blocked[2]){
			blocked[2] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[3] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
		curr.row = pos.row + i;
		if (is_valid_pos(curr) && !blocked[3]){
			blocked[3] = (board[curr.col][curr.row] != EMPTY) ? 1 : 0;
			if (board[curr.col][curr.row] == pieces[3] || board[curr.col][curr.row] == pieces[4]) return 1;
		}
	}

	// check king
	Pos kdests[8] = { { pos.col - 1, pos.row - 1 }, { pos.col - 1, pos.row }, { pos.col - 1, pos.row + 1 },
	{ pos.col + 1, pos.row - 1 }, { pos.col + 1, pos.row }, { pos.col + 1, pos.row + 1 },
	{ pos.col, pos.row - 1 }, { pos.col, pos.row + 1 } };
	for (int i = 0; i < 8; i++){
		if (is_valid_pos(kdests[i])){
			if (board[kdests[i].col][kdests[i].row] == pieces[5]) return 1;
		}
	}

	// no threat
	return 0;
}

//if the king in the color of the player is threatened
int is_check(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	char king = player == WHITE ? WHITE_K : BLACK_K;
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if (board[i][j] == king){
				Pos king_pos;
				king_pos.col = i, king_pos.row = j;
				return is_pos_threatened(king_pos, board, player);
			}
		}
	}
	return 0;
}


int piece_counter(char board[BOARD_SIZE][BOARD_SIZE], int * whites, int * blacks){
	int white_b = -1, black_b = -1, bishop_fault = 0;

	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if (get_color_by_piece(board[i][j]) == WHITE){
				whites[get_type_by_piece(board[i][j])]++;
				if (get_type_by_piece(board[i][j]) == 2){
					if (black_b != -1 && (i + j) % 2 == black_b) bishop_fault = 1;
					black_b = (i + j) % 2;
				}
			}
			else if (get_color_by_piece(board[i][j]) == BLACK){
				blacks[get_type_by_piece(board[i][j])]++;
				if (get_type_by_piece(board[i][j]) == 2){
					if (white_b != -1 && (i + j) % 2 == white_b) bishop_fault = 1;
					white_b = (i + j) % 2;
				}
			}
		}
	}
	return bishop_fault;
}

// executes a specific move on the given board
void exc_move(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color){
	board[move->dest.col][move->dest.row] = board[move->piece.col][move->piece.row];
	board[move->piece.col][move->piece.row] = EMPTY;
	char this_piece = board[move->dest.col][move->dest.row];
	if (is_EOB(move->dest, color) && (this_piece == WHITE_P || this_piece == BLACK_P)) board[move->dest.col][move->dest.row] = move->promote;
}

void clear_illegal_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	Move *prev_move = moves_head, *curr_move = moves_head;
	char tmp_board[BOARD_SIZE][BOARD_SIZE];
	while (curr_move != NULL){
		duplicate_board(board, tmp_board);
		exc_move(tmp_board, curr_move, player);
		if (is_check(tmp_board, player) != 0){
			if (curr_move == moves_head){
				moves_head = curr_move->next;
				free(curr_move);
				curr_move = moves_head, prev_move = moves_head;
			}
			else{
				prev_move->next = curr_move->next;
				free(curr_move);
				curr_move = prev_move->next;
			}
		}
		else { 
			prev_move = curr_move;
			curr_move = curr_move->next;
		}
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

int is_valid_dest(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos dest){
	if (board[dest.col][dest.row] == EMPTY) return 1;
	if (is_opposite(player, board[dest.col][dest.row])) return 2;
	return 0;
}

void get_rook_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos curr;
	curr.row = piece.row;
	curr.col = piece.col + 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col++;
	}
	curr.col = piece.col - 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col--;
	}
	curr.col = piece.col;
	curr.row = piece.row + 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.row++;
	}
	curr.row = piece.row - 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.row--;
	}
}

void get_bishop_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos curr;
	curr.col = piece.col + 1;
	curr.row = piece.row + 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col++, curr.row++;
	}
	curr.col = piece.col - 1;
	curr.row = piece.row - 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col--, curr.row--;
	}
	curr.col = piece.col - 1;
	curr.row = piece.row + 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col--, curr.row++;
	}
	curr.col = piece.col + 1;
	curr.row = piece.row - 1;
	while (is_valid_pos(curr)){
		if (is_valid_dest(board, player, curr) == 1 || is_valid_dest(board, player, curr) == 2) add_move(piece, curr, 0);
		if (is_valid_dest(board, player, curr) == 0 || is_valid_dest(board, player, curr) == 2) break;
		curr.col++, curr.row--;
	}
}

void get_pawn_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	int direction = board[piece.col][piece.row] == BLACK_P ? -1 : 1;
	int pawn_row = board[piece.col][piece.row] == BLACK_P ? 6 : 1;

	Pos dest;
	dest.col = piece.col;
	dest.row = piece.row + direction;

	if (is_valid_pos(dest)){
		if (board[dest.col][dest.row] == EMPTY){
			add_move(piece, dest, 0);
			if (is_EOB(dest, player)) for (int i = 1; i < 5; i++) add_move(piece, dest, get_piece_by_type(i, player));
		}
	}
	if (piece.row == pawn_row){
		dest.row = piece.row + (2 * direction);
		if (is_valid_pos(dest)){
			if (board[dest.col][dest.row] == EMPTY && board[piece.col][piece.row + direction] == EMPTY) add_move(piece, dest, 0);
		}
	}
	dest.col = piece.col + 1;
	dest.row = piece.row + direction;
	if (is_valid_pos(dest)){
		if (is_opposite(player, board[dest.col][dest.row])){
			add_move(piece, dest, 0);
			if (is_EOB(dest, player)) for (int i = 1; i < 5; i++) add_move(piece, dest, get_piece_by_type(i, player));
		}
	}
	dest.col = piece.col - 1;
	if (is_valid_pos(dest)){
		if (is_opposite(player, board[dest.col][dest.row])){
			add_move(piece, dest, 0);
			if (is_EOB(dest, player)) for (int i = 1; i < 5; i++) add_move(piece, dest, get_piece_by_type(i, player));
		}
	}
}

void get_moves_by_piece(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece){
	Pos* tmp_dests;
	switch (board[piece.col][piece.row]){
	case WHITE_B: case BLACK_B:
		get_bishop_moves(board, player, piece);
		return;
	case WHITE_R: case BLACK_R:
		get_rook_moves(board, player, piece);
		return;
	case WHITE_Q: case BLACK_Q:
		get_bishop_moves(board, player, piece);
		get_rook_moves(board, player, piece);
		return;
	case BLACK_P: case WHITE_P:
		get_pawn_moves(board, player, piece);
		return;
	case WHITE_K: case BLACK_K:
		tmp_dests = (Pos[8]) { { piece.col - 1, piece.row - 1 }, { piece.col - 1, piece.row }, { piece.col - 1, piece.row + 1 },
		{ piece.col + 1, piece.row - 1 }, { piece.col + 1, piece.row }, { piece.col + 1, piece.row + 1 },
		{ piece.col, piece.row - 1 }, { piece.col, piece.row + 1 }
		};
		break;
	case WHITE_N: case BLACK_N:
		tmp_dests = (Pos[8]) { { piece.col - 2, piece.row - 1 }, { piece.col - 2, piece.row + 1 },
		{ piece.col + 2, piece.row - 1 }, { piece.col + 2, piece.row + 1 },
		{ piece.col - 1, piece.row - 2 }, { piece.col + 1, piece.row - 2 },
		{ piece.col - 1, piece.row + 2 }, { piece.col + 1, piece.row + 2 }};
		break;
	default: return;
	}
	for (int i = 0; i < 8; i++){
		if (is_valid_pos(tmp_dests[i])){
			if (board[tmp_dests[i].col][tmp_dests[i].row] == EMPTY || is_opposite(player, board[tmp_dests[i].col][tmp_dests[i].row])){
				add_move(piece, tmp_dests[i], 0);
			}
		}
	}
	return;
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
	if (move->promote > 0) printf(" %s", get_piece_name_by_type(move->promote));
	printf("\n");
}


void print_moves(Move* head){
	while (head != NULL){
		print_move(head);
		head = head->next;
	}
}

void print_piece_moves(Move* head, Pos pos){
	while (head != NULL){
		if (head->piece.col == pos.col && head->piece.row == pos.row){
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

int count_moves_num(Move* head){
	int cnt = 0;
	while (head != NULL){
		cnt++;
		head = head->next;
	}
	if (cnt == 0) cnt++;
	return cnt;
}

int estimate_best_depth(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	int p_moves, op_moves, p_exp, op_exp, sum;
	int ret_depth = 0;
	clear_old_moves(moves_head);
	get_all_moves(board, !player);
	op_moves = count_moves_num(moves_head);
	clear_old_moves(moves_head);
	get_all_moves(board, player);
	p_moves = count_moves_num(moves_head);
	sum = op_moves + p_moves;
	p_exp = p_moves;
	op_exp = op_moves;
	while (sum < BOARD_LIMIT){
		ret_depth++;
		op_exp *= op_moves;
		p_exp *= p_moves;
		sum = op_exp + p_exp;
	}
	return ret_depth;
}

// calculates the score of the board from a player's prospective
int calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
	//int whites[6] = { 0 }, blacks[6] = { 0 };
	int * whites = malloc(sizeof(int) * 6);
	int * blacks = malloc(sizeof(int) * 6);
	for (int i = 0; i < 6; i++){
		whites[i] = 0;
		blacks[i] = 0;
	}
	piece_counter(board, whites, blacks);

	int white_score = whites[get_type_by_piece(WHITE_P)] + 3 * whites[get_type_by_piece(WHITE_B)] + 3 * whites[get_type_by_piece(WHITE_N)] 
		+ 5 * whites[get_type_by_piece(WHITE_R)] + 9 * whites[get_type_by_piece(WHITE_Q)] + 400 * whites[get_type_by_piece(WHITE_K)];
	int black_score = blacks[get_type_by_piece(BLACK_P)] + 3 * blacks[get_type_by_piece(BLACK_B)] + 3 * blacks[get_type_by_piece(BLACK_N)] + 
		5 * blacks[get_type_by_piece(BLACK_R)] + 9 * blacks[get_type_by_piece(BLACK_Q)] + 400 * blacks[get_type_by_piece(BLACK_K)];
	free(whites);
	free(blacks);
	//if (best_depth) board_count++;
	if (player == WHITE) return white_score - black_score;
	else return black_score - white_score;
}

//int improved_calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player){
//	// ***************************change it!
//	int whites[6] = { 0 }, blacks[6] = { 0 };
//	int pawns_scores[][] = {
//			{ 0, 0, 0, 0, 0, 0, 0, 0 },
//			{ 50, 50, 50, 50, 50, 50, 50, 50 },
//			{ 10, 10, 20, 30, 30, 20, 10, 10 },
//			{ 5, 5, 10, 27, 27, 10, 5, 5 },
//			{ 0, 0, 0, 25, 25, 0, 0, 0 },
//			{ 5, -5, -10, 0, 0, -10, -5, 5 },
//			{ 5, 10, 10, -25, -25, 10, 10, 5 },
//			{ 0, 0, 0, 0, 0, 0, 0, 0 } };
//	int knights_score[][] = {
//			{ -50, -40, -30, -30, -30, -30, -40, -50 }
//			{ -40, -20, 0, 0, 0, 0, -20, -40 },
//			{ -30, 0, 10, 15, 15, 10, 0, -30 },
//			{ -30, 5, 15, 20, 20, 15, 5, -30 },
//			{ -30, 0, 15, 20, 20, 15, 0, -30 },
//			{ -30, 5, 10, 15, 15, 10, 5, -30 },
//			{ -40, -20, 0, 5, 5, 0, -20, -40 },
//			{ -50, -40, -20, -30, -30, -20, -40, -50 }
//	};
//
//
//	int bishops_score[][] = {
//			{ -20, -10, -10, -10, -10, -10, -10, -20 },
//			{ -10, 0, 0, 0, 0, 0, 0, -10 },
//			{ -10, 0, 5, 10, 10, 5, 0, -10 },
//			{ -10, 5, 5, 10, 10, 5, 5, -10 },
//			{ -10, 0, 10, 10, 10, 10, 0, -10 },
//			{ -10, 10, 10, 10, 10, 10, 10, -10 },
//			{ -10, 5, 0, 0, 0, 0, 5, -10 },
//			{ -20, -10, -40, -10, -10, -40, -10, -20 },
//	};
//
//	int kings_start_score[][] = {
//		{ -30, -40, -40, -50, -50, -40, -40, -30 },
//		{ -30, -40, -40, -50, -50, -40, -40, -30 },
//		{ -30, -40, -40, -50, -50, -40, -40, -30 },
//		{ -30, -40, -40, -50, -50, -40, -40, -30 },
//		{ -20, -30, -30, -40, -40, -30, -30, -20 },
//		{ -10, -20, -20, -20, -20, -20, -20, -10 },
//		{ 20, 20, 0, 0, 0, 0, 20, 20 },
//		{ 20, 30, 10, 0, 0, 10, 30, 20 }
//	};
//
//	int kings_end_score[][] = {
//		{ -50, -40, -30, -20, -20, -30, -40, -50 },
//		{ -30, -20, -10, 0, 0, -10, -20, -30 },
//		{ -30, -10, 20, 30, 30, 20, -10, -30 },
//		{ -30, -10, 30, 40, 40, 30, -10, -30 },
//		{ -30, -10, 30, 40, 40, 30, -10, -30 },
//		{ -30, -10, 20, 30, 30, 20, -10, -30 },
//		{ -30, -30, 0, 0, 0, 0, -30, -30 },
//		{ -50, -30, -30, -30, -30, -30, -30, -50 }
//	};
//
//	piece_counter(board, whites, blacks);
//
//	int white_score = 100 * whites[get_type_by_piece(WHITE_P)] + 325 * whites[get_type_by_piece(WHITE_B)] + 320 * whites[get_type_by_piece(WHITE_N)]
//		+ 500 * whites[get_type_by_piece(WHITE_R)] + 975 * whites[get_type_by_piece(WHITE_Q)] + 32767 * whites[get_type_by_piece(WHITE_K)];
//	int black_score = 100 * blacks[get_type_by_piece(BLACK_P)] + 325 * blacks[get_type_by_piece(BLACK_B)] + 320 * blacks[get_type_by_piece(BLACK_N)] +
//		500 * blacks[get_type_by_piece(BLACK_R)] + 975 * blacks[get_type_by_piece(BLACK_Q)] + 32767 * blacks[get_type_by_piece(BLACK_K)];
//	
//	if (remainingPieces < 10)
//	{
//		board.EndGamePhase = true;
//
//	//if (best_depth) board_count++;
//	for (int i = 0; i < BOARD_SIZE; i++){
//		for (int j = 0; j < BOARD_SIZE; j++){
//			if (board[i][j] != EMPTY){
//				if (get_type_by_piece(board[i][j]) == 5){
//					if (get_color_by_piece(board[i][j]) == WHITE) white_score += PAWNS_SCORE[i][j];
//			}
//		}
//	}
//	if (player == WHITE) return white_score - black_score;
//	else return black_score - white_score;
//}
//

// minimax recursive func, using alpha-beta pruning
int alpha_beta_minimax(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int depth, int alpha, int beta){
	Move* move_list = get_all_moves(board, player);
	Move* curr_move = move_list;
	if (depth == 0 && curr_move == NULL){
		best_move = NULL;
		return -500;
	}
	if (curr_move == NULL && player != curr_player){
		if (!is_check(board, !curr_player)) return -450;
		else return 500;
	}
	if (curr_move == NULL && player == curr_player){
		if (!is_check(board, curr_player)) return 450;
		else return -500;
	}
	if (depth == minimax_depth || curr_move == NULL){
		clear_old_moves(move_list);
		return calc_score(board, curr_player);
	}

	if (depth == 0){
		best_move = curr_move;
		if (curr_move->next == NULL){
			best_move->score = 500;
			return 500;
		}
	}

	char init_board[BOARD_SIZE][BOARD_SIZE];
	duplicate_board(board, init_board);
	// MAX
	if (depth % 2 == 0){
		while (curr_move != NULL){
			exc_move(board, curr_move, player);
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
			exc_move(board, curr_move, player);
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
	//int whites[6] = { 0 }, blacks[6] = { 0 };
	int bishop_fault = 0;
	int * whites = malloc(sizeof(int) * 6);
	int * blacks = malloc(sizeof(int) * 6);
	for (int i = 0; i < 6; i++){
		whites[i] = 0;
		blacks[i] = 0;
	}

	bishop_fault = piece_counter(board, whites, blacks);

	if (whites[get_type_by_piece(WHITE_P)] > 8 || whites[get_type_by_piece(WHITE_K)] != 1 || whites[get_type_by_piece(WHITE_Q)] > 1 ||
		blacks[get_type_by_piece(BLACK_P)] > 8 || blacks[get_type_by_piece(BLACK_K)] != 1 || blacks[get_type_by_piece(BLACK_Q)] > 1 ||
		bishop_fault){
		free(whites);
		free(blacks);
		return 0;
	}
	for (int i = 2; i < 5; i++){
		if (whites[i] > 2 || blacks[i] > 2){
			free(whites);
			free(blacks);
			return 0;
		}
	}
	int i = 1;
	free(whites);
	free(blacks);
	for (int j = 0; j < BOARD_SIZE - 1; j++){
		if (get_type_by_piece(board[j][0]) == 5 || get_type_by_piece(board[j][BOARD_SIZE - 1]) == 5){
			return 0;
		}
	}
	return 1;
}


// checks if a move is in the valid moves list
Move * is_valid_move(Move * moves, Move * new_move){
	Move * current_move = moves;
	while (current_move != NULL){
		if ((current_move->piece.col == new_move->piece.col) &&
			(current_move->piece.row == new_move->piece.row) &&
			(current_move->dest.col == new_move->dest.col) &&
			(current_move->dest.row == new_move->dest.row) &&
			(current_move->promote == new_move->promote))
			return current_move;
		
		current_move = current_move->next;
	}
	return NULL;
}