#include "run.h"

void generateMoves()
{
	int goldDig=5,digLength=7;	
	
	for(int i=0;i<11;i++){
		for(int j=0;j<11;j++){
			if(i>1 && inRange(myRow+i, myCol-5+j) && world[myRow+i][myCol-5+j]=='*' && abs(i)+abs(j-5)<digLength){
				
				if(inRange(myRow+1,myCol+1)){
					if(j>5 && canDigRight){
						int k=2;
						while(inRange(myRow+k,myCol+1) && (world[myRow+k][myCol+1]=='.' ||world[myRow+k][myCol+1]=='*'))
							k++;
						int f=0;
						for(int p=-7+k;p<7-k;p++){
							if(inRange(myRow+k-1,myCol+1+p) && enemyPresent[myRow+k-1][myCol+1+p]>=0)
								f++;
						}
						if(f==0){
							goldDig=RIGHT;
							digLength=abs(i)+abs(j-5);
							continue;
						}
					}
				}
				if(inRange(myRow+1,myCol-1)){
					if(j<=5 && canDigLeft){
						int k=2;
						while(inRange(myRow+k,myCol-1) && (world[myRow+k][myCol-1]=='.' ||world[myRow+k][myCol-1]=='*'))
							k++;
						int f=0;
						for(int p=-7+k;p<7-k;p++){
							if(inRange(myRow+k-1,myCol+1+p) && enemyPresent[myRow+k-1][myCol-1+p]>=0)
								f++;
						}
						if(f==0){
							goldDig=LEFT;
							digLength=abs(i)+abs(j-5);
							continue;
						}
						digLength=abs(i)+abs(j-5);
						goldDig=LEFT;
						continue;
					}
				}
			}
		}
	}
	
	if(myRow+2!=FIELD_ROW_COUNT && inRange(myRow+1,myCol+1) && world[myRow+1][myCol+1]=='-' && 
	  ((inRange(myRow,myCol+1) &&enemyPresent[myRow][myCol+1]>=0 )|| 
	  (inRange(myRow,myCol+2) &&enemyPresent[myRow][myCol+2]>=0 ))) {
		canMove[RIGHT]=false;
	}
	
	if(myRow+2!=FIELD_ROW_COUNT && inRange(myRow+1,myCol-1) && world[myRow+1][myCol-1]=='-' &&
	  ((inRange(myRow,myCol-1) &&enemyPresent[myRow][myCol-1]>=0 )||
	  (inRange(myRow,myCol-2) &&enemyPresent[myRow][myCol-2]>=0 ))){
		canMove[LEFT]=false;
	}
	
	
	if(inRange(myRow,myCol+1) && enemyPresent[myRow][myCol+1]>=0){
		canMove[RIGHT]=false;
		canDigRight=false;
	}
	if(inRange(myRow,myCol-1) && enemyPresent[myRow][myCol-1]>=0){
		canMove[LEFT]=false;
		canDigLeft=false;
	}
	if(inRange(myRow-1,myCol) && enemyPresent[myRow-1][myCol]>=0){
		canMove[TOP]=false;
	}
	if(inRange(myRow+1,myCol) && enemyPresent[myRow+1][myCol]>=0){
		canMove[BOTTOM]=false;
	}
	
	
	if(inRange(myRow,myCol+2) && enemyPresent[myRow][myCol+2]>=0){
		canMove[RIGHT]=false;
		if(canDigRight){
			move="DIG_RIGHT";
			return;
		}
			
	}
	if(inRange(myRow,myCol-2) && enemyPresent[myRow][myCol-2]>=0){
		canMove[LEFT]=false;
		if(canDigRight){
			move="DIG_LEFT";
			return;
		}
	}
	
	if(inRange(myRow-2,myCol) && enemyPresent[myRow-2][myCol]>=0){
		canMove[TOP]=false;
	}
	
	if(inRange(myRow+2,myCol) && enemyPresent[myRow+2][myCol]>=0){
		canMove[BOTTOM]=false;
	}
	
	
	char temper[FIELD_ROW_COUNT][FIELD_COLUMN_COUNT];
	
	for(int i=0;i<FIELD_ROW_COUNT;i++){
		for(int j=0;j<FIELD_COLUMN_COUNT;j++){
			temper[i][j]=world[i][j];
			if(enemyPresent[i][j]>=0 )
				world[i][j]='e';
		}
	}
	
	int dfsenemy=shortestPath(myRow,myCol,'e',1);
		
	int enemyDistance=sdistance;
	int te=shortestPath(myRow,myCol,'*',1);
	
	for(int i=0;i<FIELD_ROW_COUNT;i++){
		for(int j=0;j<FIELD_COLUMN_COUNT;j++){
			world[i][j]=temper[i][j];
		}
	}
	//&& enemiesInPath<3
	if(te<5 && (sdistance>5 || sdistance<=enemyDistance) ){
		if(canMove[te]){
			move=moveName[te];
			return;
		}
	}
	
	if(goldDig==LEFT  && canDigLeft && digLength<=3){
		move=digName[goldDig];
		return;
	}
	if(goldDig==RIGHT && canDigRight && digLength<=3){
		move=digName[goldDig];
		return;
	}
	if(canDigLeft && threat[LEFT] ){
		move="DIG_LEFT";
		return;
	}
	else if(canDigRight && threat[RIGHT]){
		move="DIG_RIGHT";
		return;
	} 
	
	if(dfsenemy<5 && enemyDistance<5){
		threat[dfsenemy]=true;
		canMove[dfsenemy]=false;
	}
	
	if(goldDig==LEFT  && canDigLeft){
		move=digName[goldDig];
		return;
	}
	if(goldDig==RIGHT && canDigRight){
		move=digName[goldDig];
		return;
	}
	
	if(!approachingLeft && !approachingRight){
		for(int i=0;i<5;i++){
			if(myRow+2!=FIELD_ROW_COUNT && world[myRow+1][myCol+i]=='-' && canMove[RIGHT]){
				move="RIGHT";
				return;
			}
			if(myRow+2!=FIELD_ROW_COUNT && world[myRow+1][myCol-i]=='-' && canMove[LEFT]){
				move="LEFT";
				return;
			}
		}
	}
	
	if(te<5){
		if(canMove[te]){
			move=moveName[te];
			return;
		}
	}
	
	vector< pair< int, int > > moves;	
	if(myDirection>=0 && myDirection<4){
		if(canMove[myDirection] && !threat[myDirection]){
			move=moveName[myDirection];
			return;
		}
	}
	
	for ( int i = 0; i < 4; i++ )
		if ( canMove[ i ] && !threat[ i ])
			if(((i==LEFT && !fallLeft) || i!=LEFT) && ((i==RIGHT && !fallRight) || i!=RIGHT))
			moves.push_back( make_pair( visitTime[ myRow + dRow[ i ] ][ myCol + dCol[ i ] ], i ) );
	
	if(moves.size()==0)
	for ( int i = 0; i < 4; i++ )
		if ( canMove[ i ] && !threat[ i ])
			moves.push_back( make_pair( visitTime[ myRow + dRow[ i ] ][ myCol + dCol[ i ] ], i ) );	
	
	if ( moves.size() == 0 ){
		for ( int i = 0; i < 4; i++ )
			if ( canMove[i])
				moves.push_back( make_pair( visitTime[myRow+dRow[i]][myCol+dCol[i]],i));
	}
	
	if(moves.size()){
		random_shuffle( moves.begin(), moves.end() );
		stable_sort( moves.begin(), moves.end() );
		move = moveName[ moves.front().second ];
	}
	else
		move="NONE";
	return;
}

   
