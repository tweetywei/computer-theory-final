////////////////////////////////////////////////////////////////////////////
// Edited Time: 2019/12/06                                                //
// 1. add isLast flag to prevent it from generating more one step         //
// 2. terminate process when an error occur during receiving socket data  //
////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstdlib> 
#include "Protocol.h" 
#include "MyAI.h"

void atexit_handler(){
  // do something before exit
  fprintf(stderr, "Closing.\n"); 
  fflush(stderr);
}

int main(int argc , char **argv)
{
	Protocol protocol;  
	switch(argc){
  case 2:
	case 3:  
		if(!protocol.init_protocol(argv[argc-2], atoi(argv[argc-1]))) return 0;
		break; 
	} 
	
	MyAI myai; 
	char current_position[32], move[6];
	int  piece_count[14];
	PROTO_CLR color;
	History history;
	bool turn;  
  bool isLast; // if this step is the last step
	int time; //��ܳѾl�ɶ� ��쬰�@��(ms)

	if(argc==3 || argc==2)
	{  
    atexit(atexit_handler);
    
		protocol.init_board(piece_count, current_position, history,time);

		if(history.number_of_moves != 0)
		{
			puts("-----------Record-----------");
			for(int i=0;i<history.number_of_moves;i++)
			{
				printf("%d:%s\n",i,history.move[i]);
			}	
		} 

		myai.initBoardState(piece_count, current_position);   
		protocol.get_turn(turn, color);
		myai.color(color);  
		if(turn)                           // my turn is First
		{
			myai.generateMove(move);
			protocol.send(move);           // my flip = "b6-b6" or move = "a2-a3"
			protocol.recv(move,time,isLast);	   // my flip = "b6(K)" or move = "a2-a3";			time = �Ѿl�ɶ�(ms)		
			if(color == PCLR_UNKNOW)       // all pieces are unrevealed/hidden
			{
				color = protocol.get_color(move);
				myai.color(color); 
			}
			myai.MakeMove(move);	
			protocol.recv(move,time,isLast);      // opponent flip = "c8(g)" or move = "c4-c5";	time = �Ѿl�ɶ�(ms)		
			myai.MakeMove(move);
		}
		else                               // my turn is Second
		{ 
			protocol.recv(move,time,isLast);      // opponent flip = "c8(g)" or move = "c4-c5"		time = �Ѿl�ɶ�(ms)		
			if(color == PCLR_UNKNOW)       // all pieces are unrevealed/hidden
			{ 
				color = (protocol.get_color(move)==PCLR_BLACK)?PCLR_RED:PCLR_BLACK; 
				myai.color(color);
			}
			myai.MakeMove(move);
		}
    
    // To prevent it from generating more one step after receiving the last step
    // add isLast flag to avoid this situation
		while(!isLast)
		{  
			myai.generateMove(move);   
      protocol.send(move);           // my flip = "b6-b6" or move = "a2-a3"
      protocol.recv(move,time,isLast);      // my flip = "b6(K)" or move = "a2-a3"			time = �Ѿl�ɶ�(ms)		
      myai.MakeMove(move);           // ponder 
      if(isLast) break;
      protocol.recv(move,time,isLast);	   //												time = �Ѿl�ɶ�(ms)		
      myai.MakeMove(move);           // opponent flip = "c8(g)" or move = "c4-c5"
    }  
	} 
	return 0;
} // end main(int argc , char **argv)
