#include <random>
#include <limits>
#include "CDC_HASH.h"

CDC_HASH::CDC_HASH(void){}
CDC_HASH::~CDC_HASH(void){}
void CDC_HASH::initRandomBits(){
	std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
	std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator
                       		   //and seed it with entropy.

	//Define the distribution, by default it goes from 0 to MAX(unsigned long long)
	//or what have you.
	std::uniform_int_distribution<unsigned long long> distr;
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 32; j++){
			randomBitsTable[i][j] = distr(eng);
		}
	}
	for(int i = 0; i < 32769; i++){
		nodes[0][i].p_info.depth = nodes[1][i].p_info.depth = 0;
		nodes[0][i].key = nodes[1][i].key = 0;
	}
}
void CDC_HASH::updateHash(unsigned long long key, int m, int depth, int move, char flag, int turn){
	unsigned int index = (unsigned int)(key >> 49);
	if(nodes[turn][index].p_info.depth > depth)
		return;
	else{
		nodes[turn][index].p_info.value = m;
		nodes[turn][index].p_info.depth = depth;
		nodes[turn][index].p_info.flag = flag;
		nodes[turn][index].p_info.move = move;
		nodes[turn][index].key = key;
	}
	return;
}
bool CDC_HASH::findHashHit(unsigned long long key, int* m, int* record_move, int* depth, char* flag, int turn){
	unsigned int index = (unsigned int)(key >> 49);
	unsigned long long h_key = nodes[turn][index].key;
	if(h_key == key){
		(*m) = nodes[turn][index].p_info.value;
		(*record_move) = nodes[turn][index].p_info.move;
		(*depth) = nodes[turn][index].p_info.depth;
		(*flag) = nodes[turn][index].p_info.flag;
		return true;
	}
	return false;
}

unsigned long long CDC_HASH::getRandomRepresentation(char piece, int position){
	int row = position / 10;
	int column = position % 10;
	if(row - 1 > 3 || column - 1 > 7)
		printf("Oooooooops randomBitsTable index fault, want to get position %d?\n", position);
	return randomBitsTable[piece][(row - 1) * 8 + (column - 1)];
}
