#ifndef CHESS_LOGICS_
#define CHESS_LOGICS_

#ifndef CHESS_
#include "ChessProg.h"
#endif CHESS_

//#ifndef CHESS_UTILS_
//#include "chess_utils.h"
//#endif CHESS_UTILS_

#include <ctype.h>


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
extern char black_promotables[4];
extern char white_promotables[4];
extern int gui_mode;
extern int game_on;

//void duplicate_board(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]);

#endif CHESS_LOGICS_