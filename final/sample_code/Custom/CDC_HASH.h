#include <stdio.h>
class CDC_HASH
{
	public:
		CDC_HASH(void);
		~CDC_HASH(void);
		typedef struct CONTENT{
			int value;
			int depth;
			char flag;   //[upper_bound][lower_bound][exact]
			int move;
		} h_content;

		typedef struct HASH_NODE{
			h_content p_info;
			unsigned long long key;
		} h_node;
		void initRandomBits();
		void updateHash(unsigned long long key, int m, int depth, int move, char flag, int turn);
		bool findHashHit(unsigned long long key, int* m, int* record_move, int* depth, char* flag, int turn);
		unsigned long long getRandomRepresentation(char piece, int position);
	private:

		h_node nodes[2][32769];  //2**15 = 32768
		unsigned long long randomBitsTable[16][32];

};