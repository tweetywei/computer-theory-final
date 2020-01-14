#include "CDC_AI.h"
#define LEFT -1
#define RIGHT +1
#define DOWN +10
#define UP -10

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
  	char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool CDC_AI::flip(const char* data[], char* response){
  	char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool CDC_AI::genmove(const char* data[], char* response){
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
	this->generateMove(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
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
		}
	}
	for(int j = 0; j < 7; j++){
		if(j == 0)
			dark_list[0][j] = dark_list[1][j] = 1;
		else if(j == 6)
			dark_list[0][j] = dark_list[1][j] = 5;
		else
			dark_list[0][j] = dark_list[1][j] = 2;
	}
	//this->Pirnf_Chessboard();
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

void CDC_AI::expandMoves(int capture_list[65], int move_list[65], int* capture_list_index, int* move_list_index){
	(*capture_list_index) = 0;
	(*move_list_index) = 0;
	int move_dir[4] = {1, -1, 10, -10};
	printf("start to expand moves\n");
	//loop plist
	int i;
	for(i = 0; i < num_pieces[Color]; i++){
		int from = plist[Color][i].where;
		int piece = plist[Color][i].piece_type;
		int to = -1;
		for(int d = 0; d < 4; d++){
			to = from + move_dir[d];
			printf("checking %d --> %d\n", from, to);
			if(!is_inside(to))
				continue;
			int move_type = is_legal_by_move(from, to, Color);
			if(move_type == 0) //illegal move
				continue;
			else if(move_type == 1){
				printf("generate move %d to %d, move_list_index = %d\n", from, to, (*move_list_index));
				//can move
				move_list[(*move_list_index)++] = (from * 100) + to;
			}
			else{
				//can eat
				printf("generate capture %d to %d, capture list index = %d\n", from, to, (*capture_list_index));
				capture_list[(*capture_list_index)++] = (from * 100) + to;
			}
		}

		if(piece == C){
			printf("checking cannon motion...\n");
			for(int d = 0; d < 4; d++){
				if(to = find_jump(from,move_dir[d],Color)){
					printf("cannon capture %d to %d\n", from, to);
					capture_list[(*capture_list_index)++] = (from * 100) + to;
				}
			}
		}
	}
	printf("after expand capture list index %d move list index %d\n", (*capture_list_index), (*move_list_index));
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
	printf("# convert index...: %d-%d %d-%d--> %c%c-%c%c\n",from_index / 10, from_index % 10, to_index / 10, to_index % 10, from[0], from[1], to[0], to[1]); 
	return;
}
void CDC_AI::copy_board_postion(int src, int dst){
	board[dst].empty = board[src].empty;
	board[dst].dark = board[src].dark;
	board[dst].color = board[src].color;
	board[dst].piece = board[src].piece;
}
void CDC_AI::generateMove(char move[6]){
	int capture_list[65];
	int move_list[65];
	int capture_list_index, move_list_index;
	int move_decision = 0;
	bool have_move = false;
	expandMoves(capture_list, move_list, &capture_list_index, &move_list_index);
	printf("capture list index %d move list index %d\n", capture_list_index, move_list_index);
	if(capture_list_index > 0){
		have_move = true;
		int use_index = rand() % capture_list_index;
		move_decision = capture_list[use_index];
		printf("using capture list[%d]\n", use_index);
	}
	else if(move_list_index > 0){
		have_move = true;
		int use_index = rand() % move_list_index;
		move_decision = move_list[use_index];
		printf("using move list[%d]\n", use_index);		
	}
	else{
		//flip
		printf("flip!\n");
		for(int i = 0; i < 60; i++){
			if(is_inside(i) && !board[i].empty && board[i].dark){
				have_move = true;
				move_decision = i * 100 + i;
				break;
			}
		}
	}
	if(have_move){
		char from[3];
		char to[3];
		printf("move_decision is %d--%d\n", move_decision / 100, move_decision % 100);
		convertIndexToCharPosition(from, to, move_decision);
		printf("convert to %c%c-%c%c\n", from[0], from[1], to[0], to[1]);
		sprintf(move, "%c%c-%c%c",from[0], from[1], to[0], to[1]);
	}
	else
		sprintf(move, "NAN");
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
		printf("# call flip(): flip(%d,%d) = %d\n",src, src, board[src].piece);
		char test_src[3];
		char test_to[3];
		convertIndexToCharPosition(test_src, test_src, src * 100 + src);
		printf("# in char represenation, is %c%c\n", test_src[0], test_src[1]); 
		dark_list[new_chess_color][piece_type] -= 1;  //remove revealed chess from dark chess list
		plist[new_chess_color][num_pieces[new_chess_color]].piece_type = piece_type;  //add to plist
		plist[new_chess_color][num_pieces[new_chess_color]].where = src;
		num_pieces[new_chess_color]++;
	}
	else{
		printf("# Search call move(): move : %d-%d \n",src,dst); 
		int move_color = board[src].color;
		int move_piece_type = board[src].piece % 8;
		for(int i = 0; i < num_pieces[move_color]; i++){
			if(move_piece_type == plist[move_color][i].piece_type && src == plist[move_color][i].where){
				printf("plist: update plist[%d][%d] to %d\n", move_color, i, dst);
				plist[move_color][i].where = dst;
				break;
			}
		}

		if(!board[dst].empty){  //remove from plist
			int captured_color = board[dst].color;
			int piece_type = board[dst].piece % 8;
			for(int i = 0; i < num_pieces[captured_color]; i++){
				if(piece_type == plist[captured_color][i].piece_type && dst == plist[captured_color][i].where){
					printf("plist: remove plist[%d][%d] which is at %d\n", captured_color, i, dst);
					remove_piece(i, captured_color);
				}
			}
		}

		copy_board_postion(src, dst);
		board[src].empty = true;
		board[src].piece = EMPTY;
	}
	print_board();
	for(int i = 0; i < num_pieces[Color]; i++){
		printf("plist[%d].piece: %d\n", i, plist[Color][i].piece_type);
		printf("plist[%d].where: %d\n", i, plist[Color][i].where);
	}
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
	return;
}