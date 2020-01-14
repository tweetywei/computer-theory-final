#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "MyAI.h"
class CDC_AI
{
	enum { 
		EMPTY=0,
		RK, RG, RM, RR, RN, RC, RP,
		CLOSE,
		BK, BG, BM, BR, BN, BC, BP,
		NUM_PIECE
	};
	enum {
		NONE = 0, K, G, M, R, N, C, P, NUM_TYPE
	};
public:
	CDC_AI(void);
	~CDC_AI(void);


	// commands
	bool protocol_version(const char* data[], char* response);// 0
	bool name(const char* data[], char* response);// 1
	bool version(const char* data[], char* response);// 2
	bool known_command(const char* data[], char* response);// 3
	bool list_commands(const char* data[], char* response);// 4
	bool quit(const char* data[], char* response);// 5
	bool boardsize(const char* data[], char* response);// 6
	bool reset_board(const char* data[], char* response);// 7
	bool num_repetition(const char* data[], char* response);// 8
	bool num_moves_to_draw(const char* data[], char* response);// 9
	bool move(const char* data[], char* response);// 10
	bool flip(const char* data[], char* response);// 11
	bool genmove(const char* data[], char* response);// 12
	bool game_over(const char* data[], char* response);// 13
	bool ready(const char* data[], char* response);// 14
	bool time_settings(const char* data[], char* response);// 15
	bool time_left(const char* data[], char* response);// 16
	bool showboard(const char* data[], char* response);// 17

	void generateMove(char move[6], int turn);
	void MakeMove(const char move[6]);
	void MakeMove(int move_int_rep);

private:
	struct n_b{
	bool inside; // 1 if in the board
	bool empty; // whether it is empty
	bool dark; // whether it is dark
	int color; // 0 or 1
	char piece;
	} board[(4+2)*(8+2)];

	struct pl{
	int where;
	int piece_type;
	} plist[2][16];

	int Color;  //0R 1B 2Undefined
	int dark_list[2][8]; //[color][chess_type] = the number of dark chess with chess_type and the color 
	int num_pieces[2]; // number of revealed and alive pieces
	bool can_eat_by_move[16][16];

	int R_time;
	int B_time;
	MyAI my_interface;
	int power_table[8];
	int dark_position_list[60];
	int dark_number;
	void initBoardState();
	void initCanEatList();
	int find_jump(int from, int per_step, int turn); //return 0(cannot jump) or "to" position
	// remove the ith piece of color
	void remove_piece(int i, int color){
		num_pieces[color]--;
		if(num_pieces[color] > 0){
		// swap the last piece to the ith location
			plist[color][i] = plist[color][num_pieces[color]];
		}
	}

	bool is_inside(int index){
		return board[index].inside;
	}

	int convert_index_21(int x, int y){
		return x * 10 + y;
	}
	int is_legal_by_move(int from, int to, int turn){  //0 not legal, 1 can move, 2 can capture
		bool from_is_your_piece = (board[from].color == turn);
		if(from_is_your_piece && is_inside(to) &&(can_eat_by_move[board[from].piece][board[to].piece]))
			return 2;
		if(from_is_your_piece && is_inside(to) && board[to].empty)
			return 1;
		return 0;
	}
	char convertChessNoToChar(int index);
	int convertCharToChessNo(char c);
	void expandMoves(int capture_list[65], int move_list[65], int* capture_list_index, int* move_list_index, int turn);
	int convertCharPositionToIndex(char alpha, char number);
	void convertIndexToCharPosition(char from[3], char to[3], int index);
	void copy_board_postion(int src, int dst);
	void print_board();
	int F4(int alpha, int beta, int depth, int turn, int* best_move);
	void undoMove(int move_int_rep, int eaten);
	int evaluateBoard(int turn);
};