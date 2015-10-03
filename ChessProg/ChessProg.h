#ifndef CHESS_
#define CHESS_

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>


typedef enum { WHITE = 0, BLACK = 1 } COLOR;

typedef struct pos{
	int col;
	int row;
} Pos;

typedef struct move{
	Pos piece;
	Pos dest;
	char promote; // value 0 means no promotion
	int score;
	struct move* next;
} Move;

//#ifndef CHESS_UTILS_
//#include "chess_utils.h"
//#endif CHESS_UTILS_

#ifndef CHESS_LOGICS_
#include "chess_logics.h"
#endif CHESS_LOGICS_

#ifndef CHESS_UI_
#include "chess_ui.h"
#endif CHESS_UI_





#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_N 'n'
#define WHITE_R 'r'
#define WHITE_Q 'q'
#define WHITE_K 'k'

#define BLACK_P 'M'
#define BLACK_B 'B'
#define BLACK_N 'N'
#define BLACK_R 'R'
#define BLACK_Q 'Q'
#define BLACK_K 'K'

#define EMPTY ' '

#define BOARD_SIZE 8
#define BOARD_LIMIT 1000000

typedef char** board_t;

// board initialization
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_PIECE "Setting this piece creates an invalid board\n"  
#define NO_DISC "The specified position does not contain your piece\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"


#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"

#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CALTLING_MOVE "Illegal castling move\n"  


#define perror_message(func_name) (perror("Error: standard function %s has failed", func_name));
#define print_message(message) (printf("%s", message));

#define ENTER_YOUR_MOVE "%s player - Enter your move:\n" 
#define WHITE_WIN "MATE! White player wins!\n" 
#define BLACK_WIN "MATE! Black player wins!\n" 
#define TIE "The game ends in a tie\n"
#define CHECK "Check!\n"

#define QUIT 0
#define GAME_ON 1
#define WIN_POS 2
#define TIE_POS 3


//// Memory allocation and standard functions monitoring
//void add_to_list(void* mem);
//void remove_from_list(void* mem);
//void * safe_malloc(size_t size);
//void * safe_realloc(void *old_pointer, size_t size);
//int safe_fgetc(FILE *stream);
//void safe_free(void * mem);
//
//// Draughts code
//int is_valid_pos(Pos pos);
//int is_king(char piece);
//int is_opposite(COLOR player, char piece);
//int is_EOB(Pos piece, COLOR player);
//
//void clear_old_moves(Move* head);
//void add_move(Pos piece, Pos* dests, int move_captures);
//Pos get_next_diag(Pos from, Pos to);
//Pos get_prev_diag(Pos from, Pos to);
//int get_capture_moves(Pos start, Pos piece, char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int count, Pos* dests);
//void get_man_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
//void get_king_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, Pos piece);
//Move * get_all_moves(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
//
//void print_move(Move* head);
//void print_moves(Move* head);
//int get_piece_score(char piece, COLOR player);
//int calc_score(char board[BOARD_SIZE][BOARD_SIZE], COLOR player);
//void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]);
//int alpha_beta_minimax(char board[BOARD_SIZE][BOARD_SIZE], COLOR player, int depth, int alpha, int beta);
//
//int is_valid_board(char board[BOARD_SIZE][BOARD_SIZE]);
//void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
//void init_board(char board[BOARD_SIZE][BOARD_SIZE]);
//void clear_board(char board[BOARD_SIZE][BOARD_SIZE]);
//char* input2str(FILE* pFile);
//void exc(char* str, char board[BOARD_SIZE][BOARD_SIZE]);
//int computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color);
//int user_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color);
//int is_valid_piece(char board[BOARD_SIZE][BOARD_SIZE], Move * move, COLOR color);
//Move * is_valid_move(Move * moves, Move * new_move);
//void exc_move(char board[BOARD_SIZE][BOARD_SIZE], Move * move);

// Globals
extern COLOR user_color;
//extern int minimax_depth;
//extern Move* moves;
//extern Move* moves_head;
//extern char curr_piece;
extern COLOR curr_player;
//extern Move* best_move;

#endif CHESS_



