//#ifndef CHESS_
#define CHESS_

#define _CRT_SECURE_NO_WARNINGS

#ifndef CHESS_UTILS_
#include "chess_utils.h"
#endif CHESS_UTILS_

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

#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define WRONG_PIECE "Setting this piece creates an invalid board\n"  
#define NO_PIECE "The specified position does not contain your piece\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"

#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CALTLING_MOVE "Illegal castling move\n"  

#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));

#define ENTER_YOUR_MOVE "%s player - Enter your move:\n" 
#define WHITE_WIN "MATE! White player wins!\n" 
#define BLACK_WIN "MATE! Black player wins!\n" 
#define TIE "The game ends in a tie\n"
#define CHECK "Check!\n"

#define QUIT 0
#define GAME_ON 1
#define LOSE_POS 2
#define TIE_POS 3
#define CHECK_POS 4

#define malloc(x) safe_malloc(x)
#define realloc(x, y) safe_realloc((x), (y))
#define fgetc(x) safe_fgetc(x)
#define free(x) safe_free(x)

#define printf(...) \
	if (printf(__VA_ARGS__) < 0){ \
		perror_message("printf"); \
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]); \
		abort();} \
				else (void)0

void print_line();
void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
void clear_board(char board[BOARD_SIZE][BOARD_SIZE]);
void init_board(char board[BOARD_SIZE][BOARD_SIZE]);
int get_line_by_node_name(char * name);
int load_game(char * path, char board[BOARD_SIZE][BOARD_SIZE]);
int save_game(char board[BOARD_SIZE][BOARD_SIZE], COLOR color, char * file_name);
char* input2str(FILE* pFile);
void conosle_settings_mode(char* str, char board[BOARD_SIZE][BOARD_SIZE]);
int pre_turn_verify(char board[BOARD_SIZE][BOARD_SIZE], COLOR color);
void computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color);
void get_best_moves(char board[BOARD_SIZE][BOARD_SIZE], int depth);
void user_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color);
void console_alert(int alert);
void gui_alert(int alert);
int main(int argc, char * argv[]);

//#endif CHESS_



