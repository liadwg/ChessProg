#ifndef CHESS_LOGICS_
#define CHESS_LOGICS_

//#include <ctype.h>

#ifndef CHESS_UTILS_
#include "chess_utils.h"
#endif CHESS_UTILS_

#ifndef CHESS_
#include "ChessProg.h"
#endif CHESS_

//extern Move* moves;
//extern Move* moves_head;
//extern char curr_piece;
//extern COLOR curr_player;
//extern Move* best_move;
//extern COLOR user_color;
//extern COLOR start_color; 
//extern int minimax_depth;
//extern int game_mode;
//extern int best_depth;
//extern int board_count;
//extern char black_promotables[4];
//extern char white_promotables[4];
//extern int gui_mode;
//extern int game_on;

//void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]);



//// Pointer list management
//void add_to_list(void* mem);
//void remove_from_list(void* mem);
//
//// safe_funcs verifies that that the original functions succeeded
//void * safe_malloc(size_t size);
//void * safe_realloc(void *old_pointer, size_t size);
//int safe_fgetc(FILE *stream);
//void safe_free(void * mem);

#define BOARD_SIZE 8

#define malloc(x) safe_malloc(x)
#define realloc(x, y) safe_realloc((x), (y))
#define fgetc(x) safe_fgetc(x)
#define free(x) safe_free(x)

//#define printf(...) \
//	if (printf(__VA_ARGS__) < 0){ \
//		perror_message("printf"); \
//		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]); \
//		abort();} \
//		else (void)0

#define printf(...) \
	if (printf(__VA_ARGS__) < 0){ \
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]); \
		abort();} \
				else (void)0

//extern void* mem_list[5000];
//extern int mem_count;
//extern int fail_safe;

// Globals
extern Move* moves;
extern Move* moves_head;
extern char curr_piece;
extern COLOR curr_player;
extern Move* best_move;
extern COLOR user_color;
extern COLOR start_color;
extern int minimax_depth;
extern int game_mode;
extern int best_depth;
extern int board_count;
extern int gui_mode;
extern int game_on;
extern char black_promotables[4];
extern char white_promotables[4];


// funcs
int is_valid_pos(Pos pos);
int is_opposite(COLOR player, char piece);
char* get_piece_name_by_type(int type);
int get_type_by_name(char * piece);
char get_piece_by_name(char * name, COLOR color);
char get_piece_by_type(int type, COLOR player);
int get_type_by_piece(char piece);
COLOR get_color_by_piece(char piece);
int is_valid_piece(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color);
int is_EOB(Pos piece, COLOR player);
void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]);
int is_pos_threatened(Pos pos, char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
int is_check(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
int piece_counter(char board[BOARD_SIZE][BOARD_SIZE], int * whites, int * blacks);
void exc_move(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color);
void clear_illegal_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
void clear_old_moves(Move* head);
void add_move(Pos piece, Pos dest, char promote);
int is_valid_dest(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos dest);
void get_rook_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
void get_bishop_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
void get_pawn_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
void get_moves_by_piece(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
Move * get_all_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
void print_move(Move* move);
void print_moves(Move* head);
void print_piece_moves(Move* head, Pos pos);
void print_best_moves(Move* head, int score);
int count_moves_num(Move* head);
int estimate_best_depth(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
int calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
//int improved_calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
int alpha_beta_minimax(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int depth, int alpha, int beta);
int is_valid_board(char board[BOARD_SIZE][BOARD_SIZE]);
Move * is_valid_move(Move * moves, Move * new_move);


#endif CHESS_LOGICS_