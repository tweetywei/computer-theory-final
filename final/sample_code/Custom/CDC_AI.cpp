#include "CDC_AI.h"
#define LEFT -1
#define RIGHT +1
#define DOWN +10
#define UP -10
#define CURRENT_DEPTH 6
CDC_AI::CDC_AI(void){this->Color = 2;}

CDC_AI::~CDC_AI(void){}

bool CDC_AI::protocol_version(const char* data[], char* response){
	strcpy(response, "1.0.0");
  return 0;
}

bool CDC_AI::name(const char* data[], char* response){
	strcpy(response, "CDC_AI");
	return 0;
}

bool CDC_AI::version(const char* data[], char* response){
	strcpy(response, "1.0.0");
	return 0;
}

bool CDC_AI::known_command(const char* data[], char* response){
  return my_interface.known_command(data, response);
}

bool CDC_AI::list_commands(const char* data[], char* response){
  return my_interface.list_commands(data, response);
}

bool CDC_AI::quit(const char* data[], char* response){
	return my_interface.quit(data, response);
}

bool CDC_AI::boardsize(const char* data[], char* response){
  	fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]); 
	return 0;
}

bool CDC_AI::reset_board(const char* data[], char* response){
	this->Color = 2;
	this->R_time = -1; // known
	this->B_time = -1; // known
	this->initBoardState();
	this->initCanEatList();
	return 0;
}

bool CDC_AI::num_repetition(const char* data[], char* response){
  return 0;
}

bool CDC_AI::num_moves_to_draw(const char* data[], char* response){
  return 0;
}

bool CDC_AI::move(const char* data[], char* response){
	printf("calling move\n");
  	char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool CDC_AI::flip(const char* data[], char* response){
	printf("calling flip\n");
  	char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool CDC_AI::genmove(const char* data[], char* response){
	printf("calling genmove\n");
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = 0;
	}else if(!strcmp(data[0], "black")){
		this->Color = 1;
	}else{
		this->Color = 2;
	}
	// genmove
  char move[6];
	this->generateMove(move, Color);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	print_board();
	print_alive(Color);
	return 0;
}

bool CDC_AI::game_over(const char* data[], char* response){
  fprintf(stderr, "Game Result: %s\n", data[0]); 
	return 0;
}

bool CDC_AI::ready(const char* data[], char* response){
  return 0;
}

bool CDC_AI::time_settings(const char* data[], char* response){
  return 0;
}

bool CDC_AI::time_left(const char* data[], char* response){
  if(!strcmp(data[0], "red")){
		sscanf(data[1], "%d", &(this->R_time));
	}else{
		sscanf(data[1], "%d", &(this->B_time));
	}
	fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]); 
	return 0;
}

bool CDC_AI::showboard(const char* data[], char* response){
  //Pirnf_Chessboard();
	return 0;
}


void CDC_AI::initBoardState ()
{
	num_pieces[0] = num_pieces[1] = 0;

	//convert to my format
	int Index = 0;
	dark_number = 0;
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<6;j++)
		{
			int this_index = convert_index_21(j, i);
			board[this_index].inside = true;
			board[this_index].empty = false;
			board[this_index].dark = true;
			board[this_index].color = 2;
			board[this_index].piece = CLOSE;
			if(j == 0 || i == 0 || i == 9 || j == 5){
				board[this_index].inside = false;
				board[this_index].empty = true;
			}
			else{
				dark_position_list[dark_number++] = this_index;
			}
		}
	}
	for(int j = K; j < NUM_TYPE; j++){
		if(j == 0)
			dark_list[0][j] = dark_list[1][j] = 1;
		else if(j == 6)
			dark_list[0][j] = dark_list[1][j] = 5;
		else
			dark_list[0][j] = dark_list[1][j] = 2;
	}
	power_table[NONE] = 0;
	power_table[P]  = 1;
	power_table[N]  = 6;
	power_table[R]  = 18;
	power_table[M]  = 54;
	power_table[G]  = 162;
	power_table[K]  = 486;
	power_table[C]  = 100;
	//this->Pirnf_Chessboard();
	my_hash.initRandomBits();
	current_hash = 0;
	for(int i = 0; i < 60; i++){
		if(is_inside(i))
			current_hash ^= my_hash.getRandomRepresentation(CLOSE, i);
	}
	print_board();
}

void CDC_AI::initCanEatList(){
	for(int i = 0; i < NUM_PIECE; i++)
		for(int j = 0; j < NUM_PIECE; j++)
			can_eat_by_move[i][j] = false;

	for(int i = 1; i < 8; i++){
		for(int j = 9; j < NUM_PIECE; j++){
			int j_power = j - 8;
			if(i <= j_power && !(i == 1 && j_power == 7)){
				can_eat_by_move[i][j] = true;  // large eat small, but king cannot eat pawn
			}
			if(j_power <= i && !(j_power == 1 && i == 7)){
				can_eat_by_move[j][i] = true;
			}
			can_eat_by_move[RC][j] = false; //cannon can not eat by move
		}
		can_eat_by_move[BC][i] = false; //cannon can not eat by move
	}
	can_eat_by_move[RP][BK] = true;
	can_eat_by_move[BP][RK] = true;
}

int CDC_AI::find_jump(int from, int per_step, int turn){
	bool find_pivot = false;
	int current_to = from;
	while(true){
		current_to += per_step;
		if(!is_inside(current_to))
			break;
		if(find_pivot == false && !board[current_to].empty)
			find_pivot = true;
		else if(find_pivot && !board[current_to].empty && !board[current_to].dark && (board[current_to].color != turn))
			return current_to;
		else if(find_pivot && !board[current_to].empty)
			break;
	}
	return 0;
}

void CDC_AI::expandMoves(int capture_list[100], int move_list[100], int* capture_list_index, int* move_list_index, int turn){
	(*capture_list_index) = 0;
	(*move_list_index) = 0;
	int move_dir[4] = {1, -1, 10, -10};
	//printf("start to expand moves\n");
	//loop plist
	int i;
	for(i = 0; i < num_pieces[turn]; i++){
		int from = plist[turn][i].where;
		int piece = plist[turn][i].piece_type;
		int to = -1;
		for(int d = 0; d < 4; d++){
			to = from + move_dir[d];
			//printf("checking %d --> %d\n", from, to);
			if(!is_inside(to))
				continue;
			int move_type = is_legal_by_move(from, to, turn);
			if(move_type == 0) //illegal move
				continue;
			else if(move_type == 1){
				//printf("generate move %d to %d, move_list_index = %d\n", from, to, (*move_list_index));
				fflush(stdout);
				//can move
				move_list[(*move_list_index)++] = (from * 100) + to;
			}
			else{
				//can eat
				//printf("generate capture %d to %d, capture list index = %d\n", from, to, (*capture_list_index));
				fflush(stdout);
				capture_list[(*capture_list_index)++] = (from * 100) + to;
			}
		}

		if(piece == C){
			//printf("checking cannon motion...\n");
			fflush(stdout);
			for(int d = 0; d < 4; d++){
				if(to = (find_jump(from,move_dir[d],turn))){
					//printf("cannon capture %d to %d\n", from, to);
					fflush(stdout);
					capture_list[(*capture_list_index)++] = (from * 100) + to;
				}
			}
		}
	}
	//printf("after expand capture list index %d move list index %d\n", (*capture_list_index), (*move_list_index));
	fflush(stdout);
	return;
}

int CDC_AI::convertCharToChessNo(char c)
{
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	for(int f=0;f<16;f++)if(c==skind[f])return f;
	return -1;
}

char CDC_AI::convertChessNoToChar(int index){
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	return skind[index];
}

int CDC_AI::convertCharPositionToIndex(char alpha, char number){
/*	8				 0	1	2	3	4	5	6	7	8	9	
	7		   -->	10	11	12	13	14	15	16...
	6				   (a1)(a2)(a3)
	5				 
	4
	3
	2
	1
	 a b c d 												*/
	return (alpha - 'a' + 1) * 10 + (number - '0');
}
void CDC_AI::convertIndexToCharPosition(char from[3], char to[3], int index){
	//index = from * 100 + to
	int from_index = index / 100;
	int to_index = index % 100;
	from[0] = (from_index / 10 - 1) + 'a';
	from[1] = (from_index % 10) + '0';
	to[0] = (to_index / 10 - 1) + 'a';
	to[1] = (to_index % 10) + '0';
	//printf("# convert index...: %d-%d %d-%d--> %c%c-%c%c\n",from_index / 10, from_index % 10, to_index / 10, to_index % 10, from[0], from[1], to[0], to[1]); 
	return;
}
void CDC_AI::copy_board_postion(int src, int dst){
	board[dst].empty = board[src].empty;
	board[dst].dark = board[src].dark;
	board[dst].color = board[src].color;
	board[dst].piece = board[src].piece;
}

int CDC_AI::evaluateBoard(int turn){
	int total_power = 0;
	for(int i = 0; i < num_pieces[turn]; i++){
		total_power += power_table[plist[turn][i].piece_type];
		//printf("adding piece_type %d, power is %d\n", plist[turn][i].piece_type, power_table[plist[turn][i].piece_type]);
	}
	for(int i = 0; i < num_pieces[(turn+1)%2]; i++){
		total_power -= power_table[plist[(turn+1)%2][i].piece_type];
		//printf("adding piece_type %d, power is %d\n", plist[turn][i].piece_type, power_table[plist[turn][i].piece_type]);
	}	//fflush(stdout);

	return total_power;
}
int CDC_AI::Star1_F3(int position, int alpha, int beta, int depth, int turn, int* best_move){
	int vmax = power_table[K] + evaluateBoard(turn);
	int M0 = vmax;
	int A0 = dark_number * (alpha - vmax) + vmax;
	int vmin = evaluateBoard(turn) - power_table[K] > 0;
	int B0 = dark_number * (beta - vmin) + vmin;
	int m0 = vmin;
	int vsum = 0;
	int next_best = 0;
	for(int i = 0; i < 16; i++){
		int flip_color = i / 8;
		int flip_piece_type = i % 8;
		if(i % 8 == 0)
			continue;
		if(dark_list[i / 8][i % 8] <= 0)
			continue;
		int piece_number = dark_list[flip_color][flip_piece_type];
		MakeMove(position * 100 + (flip_color * 8 + flip_piece_type), 1);  //remember to deal with dark_list[turn][i] > 1
		int t = F4((-1) * std::min(B0, vmax), (-1) * std::max(A0, vmin), depth - 1, (turn + 1)%2, &next_best);
		undoMove(position * 100 + (flip_color * 8 + flip_piece_type), 0, 1);
		m0 += (t * piece_number - vmin * piece_number) / dark_number;
		M0 += (t * piece_number - vmax * piece_number) / dark_number;
		if(t >= B0) return m0;
		if(t <= A0) return M0;
		vsum += piece_number * t;
		A0 += piece_number * (vmax - t);
		B0 += piece_number * (vmin - t);
	}
	return vsum / dark_number;
}
int CDC_AI::F4(int alpha, int beta, int depth, int turn, int* best_move){
	//printf("calling F4, depth = %d, turn = %d\n", depth, turn);
	if(depth <= 0)
		return evaluateBoard(turn);
	int capture_list[100];
	int move_list[100];
	int capture_list_index = 0;
	int move_list_index = 0;

	int hash_m;
	int hash_record;
	int hash_d;
	char hash_flag;

	int m = -2147483647;
	int n = beta;
	if(my_hash.findHashHit(current_hash, &hash_m, &hash_record, &hash_d, &hash_flag, turn)){
		//check flag
		if(is_legal_by_move(hash_record / 100, hash_record % 100, turn)){
			printf("ohoh find hash!!!\n");
			m = hash_m;
			(*best_move) = hash_record;
		}
	}

	expandMoves(capture_list, move_list, &capture_list_index, &move_list_index, turn);	
	if(capture_list_index + move_list_index<= 0){
		int score = evaluateBoard(turn);
		//printf("return from F4, depth = %d, turn = %d, score = %d\n", depth, turn, score);
		fflush(stdout);
		return score;
	}
	//printf("get return from evaluating...\n");
	fflush(stdout);
	int next_best;

	for(int i = 0; i < capture_list_index + move_list_index; i++){
		int eaten = 0;
		int try_move;
		if(i < capture_list_index){
			try_move = capture_list[i];
			eaten = board[try_move % 100].piece;
		}
		else if(i < capture_list_index + move_list_index){
			try_move = move_list[i - capture_list_index];
		}
		MakeMove(try_move, 0);
		//do move
		int t = (-1) * F4((-1) * n, (-1) * std::max(alpha, m), depth - 1, (turn + 1) % 2, &next_best);
		fflush(stdout);

		if(t > m){
			if(n == beta || depth < 3 || t >= beta)
				m = t;
			else{
				m = (-1) * F4((-1) * beta, (-1) * t, depth - 1, (turn + 1) % 2, &next_best);
				fflush(stdout);
			}
			(*best_move) = try_move;
			//printf("get m = %d from move %d\n", m, try_move);
			fflush(stdout);
		}
		undoMove(try_move, eaten, 0);
		if(m >= beta){
			//printf("return from F4, depth = %d, turn = %d, score = %d, a cut happens\n", depth, turn, m);
			char flag = 2;
			my_hash.updateHash(current_hash, m, depth, (*best_move), flag, turn);
			return m;
		}
		n = std::max(alpha, m) + 1;
	}
	//printf("return from F4, depth = %d, turn = %d, score = %d\n", depth, turn, m);
	if(dark_number < 5){
		for(int i = 0; i < dark_number; i++){
			//flip chess
			int flip_position = dark_position_list[i];
			int t = Star1_F3(flip_position, alpha, beta, 2, turn, best_move);
			if(t > m)
				(*best_move) = flip_position*100 + flip_position;
			if(m >= beta){
				//printf("return from F4, depth = %d, turn = %d, score = %d, a cut happens\n", depth, turn, m);
				char flag = 2;
				my_hash.updateHash(current_hash, m, depth, (*best_move), flag, turn);
				return m;
			}
		n = std::max(alpha, m) + 1;
		}
	}

	if(m >= alpha){
		char flag = 1;
		my_hash.updateHash(current_hash, m, depth, (*best_move), flag, turn);
	}else{
		char flag = 4;
		my_hash.updateHash(current_hash, m, depth, (*best_move), flag, turn);
	}
	return m;
}

void CDC_AI::generateMove(char move[6], int turn){
	int best_move = 0;
	bool have_move = false;
	int return_F4 = F4(-2147483647, 2147483647, CURRENT_DEPTH, turn, &best_move);

	if(best_move){ 
		have_move = true;
		fflush(stdout);
	}
	if(!best_move && dark_number > 0){
		//random flip
		have_move = true;
		int flip_pos = dark_position_list[rand() % dark_number];
		best_move = flip_pos * 100 + flip_pos;
		fflush(stdout);
	}
	if(have_move){
		char from[3];
		char to[3];
		//printf("move_decision is %d--%d\n", best_move / 100, best_move % 100);
		convertIndexToCharPosition(from, to, best_move);
		printf("convert to %c%c-%c%c\n", from[0], from[1], to[0], to[1]);
		sprintf(move, "%c%c-%c%c",from[0], from[1], to[0], to[1]);	
	}
	else
		sprintf(move, "NAN");	
	return;
}

void CDC_AI::undoMove(int move_int_rep, int eaten, int is_flip){
	if(is_flip){
		int src = move_int_rep / 100;
		char flip_piece = (move_int_rep % 100); 
		int piece_color = (move_int_rep % 100) / 8;
		int piece_type = (move_int_rep % 100) % 8;
		current_hash ^= my_hash.getRandomRepresentation(flip_piece, src) ^ my_hash.getRandomRepresentation(CLOSE, src);
		//printf("undo flip (%d,%d) = %d\n", src, src, move_int_rep % 100);
		this->board[src].piece = CLOSE;
		this->board[src].dark = true;
		this->board[src].color = 2;

		dark_list[piece_color][piece_type] += 1;
		for(int i = 0; i < num_pieces[piece_color]; i++){
			if(plist[piece_color][i].piece_type == piece_type && plist[piece_color][i].where == src){
				remove_piece(i, piece_color);
				break;
			}
		}
		dark_position_list[dark_number++] = src;
		return;
	}
	int src = move_int_rep / 100;
	int dst = move_int_rep % 100;
	//printf("undo move: %d --> %d\n", dst, src);
	fflush(stdout);
	int move_color = board[dst].color;
	int move_piece_type = board[dst].piece % 8;
	char move_piece = board[dst].piece;
	copy_board_postion(dst, src);
	current_hash ^= my_hash.getRandomRepresentation(move_piece, src) ^ my_hash.getRandomRepresentation(move_piece, dst) ^ my_hash.getRandomRepresentation(EMPTY, src);

	for(int i = 0; i < num_pieces[move_color]; i++){
		if(move_piece_type == plist[move_color][i].piece_type && dst == plist[move_color][i].where){
				//printf("plist: undo plist[%d][%d] to %d\n", move_color, i, src);
				plist[move_color][i].where = src;
				break;
		}
	}
	if(eaten){
		int eaten_color = eaten / 8;
		plist[eaten_color][num_pieces[eaten_color]].piece_type = eaten % 8;
		plist[eaten_color][num_pieces[eaten_color]++].where = dst;
		board[dst].piece = eaten;
		board[dst].empty = false;
		board[dst].color = eaten_color;
		current_hash ^= my_hash.getRandomRepresentation(eaten, dst);
	}
	else{
		board[dst].empty = true;
		board[dst].piece = EMPTY;
		current_hash ^= my_hash.getRandomRepresentation(EMPTY, dst);
	}
	return;
}

void CDC_AI::MakeMove(const char move[6]){
	int src = convertCharPositionToIndex(move[0], move[1]);
	int dst = convertCharPositionToIndex(move[3], move[4]);
	if(move[2]=='('){
		int new_chess = convertCharToChessNo(move[3]);
		int new_chess_color = new_chess / 8;
		int piece_type = new_chess % 8;
		this->board[src].piece = new_chess;
		this->board[src].dark = false;
		this->board[src].color = (new_chess > 8)? 1: 0;
		//printf("# call flip(): flip(%d,%d) = %d==========================\n",src, src, board[src].piece);
		fflush(stdout);
		char test_src[3];
		char test_to[3];
		convertIndexToCharPosition(test_src, test_src, src * 100 + src);
		//printf("# in char represenation, is %c%c\n", test_src[0], test_src[1]); 
		dark_list[new_chess_color][piece_type] -= 1;  //remove revealed chess from dark chess list
		plist[new_chess_color][num_pieces[new_chess_color]].piece_type = piece_type;  //add to plist
		plist[new_chess_color][num_pieces[new_chess_color]].where = src;
		num_pieces[new_chess_color]++;
		for(int l = 0; l < dark_number; l++){
			if(dark_position_list[l] == src){
				dark_number--;
				dark_position_list[l] = dark_position_list[dark_number];
				break;
			}
		}
		current_hash ^= my_hash.getRandomRepresentation((char)new_chess, src) ^ my_hash.getRandomRepresentation(CLOSE, src);
	}
	else{
		//printf("# Search call move(): move : %d-%d \n",src,dst); 
		fflush(stdout);
		int move_color = board[src].color;
		int move_piece_type = board[src].piece % 8;
		for(int i = 0; i < num_pieces[move_color]; i++){
			if(move_piece_type == plist[move_color][i].piece_type && src == plist[move_color][i].where){
				//printf("plist: update plist[%d][%d] to %d\n", move_color, i, dst);
				fflush(stdout);
				plist[move_color][i].where = dst;
				break;
			}
		}

		if(!board[dst].empty){  //remove from plist
			int captured_color = board[dst].color;
			int piece_type = board[dst].piece % 8;
			char capture_piece = board[dst].piece;
			current_hash ^= my_hash.getRandomRepresentation(capture_piece, dst);
			for(int i = 0; i < num_pieces[captured_color]; i++){
				if(piece_type == plist[captured_color][i].piece_type && dst == plist[captured_color][i].where){
					//printf("plist: remove plist[%d][%d] which is at %d\n", captured_color, i, dst);
					fflush(stdout);
					remove_piece(i, captured_color);
					break;
				}
			}
		}else
			current_hash ^= my_hash.getRandomRepresentation(EMPTY, dst);

		char move_piece = board[src].piece;
		//move to dsts
		current_hash ^= my_hash.getRandomRepresentation(EMPTY, src) ^ my_hash.getRandomRepresentation(move_piece, src) ^ my_hash.getRandomRepresentation(move_piece, src);
		
		copy_board_postion(src, dst);
		board[src].empty = true;
		board[src].piece = EMPTY;
	}
	//print_board();
}

void CDC_AI::MakeMove(int move_int_rep, int is_flip){
	if(is_flip){
		char from[3];
		char to[3];
		int flip_index = (move_int_rep / 100) * 100 + (move_int_rep / 100);
		convertIndexToCharPosition(from, to, flip_index);
		char move[6];
		sprintf(move, "%c%c(%c)", from[0], from[1], convertChessNoToChar(move_int_rep % 100));
		MakeMove(move);
		return;
	}
	char from[3];
	char to[3];
	convertIndexToCharPosition(from, to, move_int_rep);
	char move[6];
	sprintf(move, "%c%c %c%c", from[0], from[1], to[0], to[1]);
	MakeMove(move);
	return;
}

void CDC_AI::print_board(){
	for(int i = 8; i > 0; i--){
		printf("\n");
		printf("%d ", i);
		for(int j = 1; j < 5; j++){
			if(!is_inside(convert_index_21(j, i))){
				printf("? ");
				continue;
			}
			char this_chess = convertChessNoToChar(board[convert_index_21(j, i)].piece);
			printf("%c ", this_chess);
		}
	}
	printf("\n");
	printf("  a b c d\n");
	fflush(stdout);
	return;
}

void CDC_AI::print_alive(int turn){
	printf("=====print alive pieces=========\n");
	for(int i = 0; i < num_pieces[turn]; i++){
		printf("%d at %d; ", plist[turn][i].piece_type, plist[turn][i].where);
	}
	printf("================================\n");
}