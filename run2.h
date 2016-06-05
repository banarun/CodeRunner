#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

// Size of the playing field.
const int FIELD_ROW_COUNT = 16;
const int FIELD_COLUMN_COUNT = 25;

// How close an enemy can be before we try to move away or trap it.
const int THREAT_THRESHOLD = 5;

// Constants for the various directions the player can move.
enum MoveDir { LEFT, TOP, RIGHT, BOTTOM };

// Names for the different directions.
string moveName[] = { "LEFT", "TOP", "RIGHT", "BOTTOM" };
string digName[]={"DIG_LEFT", "NONE", "DIG_RIGHT"};

// Initial positions and current positions for each runner.
int initRunnersx[2], runnersx[2];
int initRunnersy[2], runnersy[2];
int enemiesInPath;
// Current score in the game.
int scores[ 2 ];

bool fallLeft=false;
bool fallRight=false;

// How long until each runner can dig.
int digDelay[ 2 ];

// How long will the game be?
int turns;

// Read a picture of the map.
string world[ FIELD_ROW_COUNT ];

// Time whn we last visited this space, or -1.
vector< vector< int > > visitTime( FIELD_ROW_COUNT, vector< int >( FIELD_COLUMN_COUNT, -1 ) );


// Read initial positions of all the enemies.
int enemyCount;

// Respawn spot for each enemy.
vector<int> initEnemiesx(100);
vector<int> initEnemiesy(100);

// What player each enemy is afraid of (or -1), not used in this player.
vector< int > master( 100, -1 );

// Circuit the enemy will make while patrolling.  Not useed in this player.
vector<string> program(100);

int goForGold[10];
// Current enemy positions.
vector<int> enemiesx(100);
vector<int> enemiesy(100);

// Last known move direction for each enemy.
vector< int > enemyMoveDir(100);

// Can we move left, up, right or down?
vector< bool > canMove( 4, false );

// Is there a nearby enemy left, up, right or down?
vector< bool > threat( 4, false );
int enemyPresent[FIELD_ROW_COUNT][FIELD_COLUMN_COUNT];

bool approachingLeft = false, approachingRight = false;
string move;

// Can we dig left or right?
bool canDig;
bool canDigLeft = false, canDigRight = false;
bool fall[4];
int pathFollowed[100];
// Row and column offsets for the move directions
int dRow[] = { 0, -1, 0, 1 };
int dCol[] = { -1, 0, 1, 0 };

int turn,myCol,myRow,prevmyRow,prevmyCol;
int myDirection;
vector<int> prevEnemiesx;
vector<int> prevEnemiesy;

bool inRange(int x,int y)
{
	if(x<0 || x>=FIELD_ROW_COUNT || y<0 || y>=FIELD_COLUMN_COUNT)
		return false;
	return true;
}
int sdistance;
int shortestPath(int sx,int sy,char target,int num)
{

	int begin=0,end=1;
	int visited[FIELD_ROW_COUNT][FIELD_COLUMN_COUNT];
	for(int i=0;i<FIELD_ROW_COUNT;i++){
		for(int j=0;j<FIELD_COLUMN_COUNT;j++){
			visited[i][j]=0;	
		}
	}
	
	int parent[100];
	int fallflag=0;
	int goForGold[10];
	int qux[100],quy[100];
	qux[0]=sx;
	quy[0]=sy;
	int r=0;
	
	parent[0]=0;
	while(begin<end){
		if(!inRange(qux[begin],quy[begin]) || visited[qux[begin]][quy[begin]]){
			begin++;
			continue;
		}
		visited[qux[begin]][quy[begin]]++;
		
		if(world[qux[begin]][quy[begin]]==target){
			goForGold[r]=begin;
			world[qux[begin]][quy[begin]]='.';
			r++;
			if(r==num)
				break;
		}
		
		//fall down
		if(inRange(qux[begin]+1,quy[begin]) && world[qux[begin]][quy[begin]]!='H' &&
		  (world[qux[begin]+1][quy[begin]]=='.' || world[qux[begin]+1][quy[begin]]==target)){
			if(!visited[qux[begin]+1][quy[begin]]){
			//	cout<<qux[begin]<<" "<<quy[begin]<<endl;
				qux[end]=qux[begin]+1;
				quy[end]=quy[begin];
				parent[end]=begin;
				end++;
			}
			begin++;
			continue;
		}
		
		//move right
		if(inRange(qux[begin]+1,quy[begin]+1) && !visited[qux[begin]][quy[begin]+1] &&
		  (world[qux[begin]][quy[begin]+1]!='=')){
			qux[end]=qux[begin];
			quy[end]=quy[begin]+1;
			parent[end]=begin;
			end++;
		}
		//move left
		if(inRange(qux[begin]+1,quy[begin]-1)&& !visited[qux[begin]][quy[begin]-1] && 
		  (world[qux[begin]][quy[begin]-1]!='=')){
			qux[end]=qux[begin];
			quy[end]=quy[begin]-1;
			parent[end]=begin;
			end++;
		}
		//move down
		if(inRange(qux[begin]+1,quy[begin]) && !visited[qux[begin]+1][quy[begin]]&& 
		  (world[qux[begin]+1][quy[begin]]!='=') && 
		  (world[qux[begin]][quy[begin]]=='H'|| world[qux[begin]][quy[begin]]=='.')){
			qux[end]=qux[begin]+1;
			quy[end]=quy[begin];
			parent[end]=begin;
			end++;
		}
		//move up
		if(inRange(qux[begin]-1,quy[begin]) && !visited[qux[begin]-1][quy[begin]]&& 
		(world[qux[begin]-1][quy[begin]]=='H' || world[qux[begin]-1][quy[begin]]==target || world[qux[begin]-1][quy[begin]]=='.')&& 
		world[qux[begin]][quy[begin]]=='H'){
			qux[end]=qux[begin]-1;
			quy[end]=quy[begin];
			parent[end]=begin;
			end++;
		}
		begin++;
	}
	sdistance=100;
	//cout<<endl;
	if(r>0){
		int temp=goForGold[0];
		int p=temp;
		int d=0;
		
		while(temp){
			p=temp;
			temp=parent[temp];
			d++;
		}
		sdistance=d;
		//cout<<"P="<<p<<" "<<qux[p]<<" "<<quy[p]<<endl;
		if(sy-quy[p]>0){
			return LEFT;
		}
		else if(sy-quy[p]<0){
			return RIGHT;
		}
		else if(sx-qux[p]>0){
			return TOP;
		}
		else if(sx-qux[p]<0){
			return BOTTOM;
		}
		else
		return 5;
	}
	return 6;
}

void init()
{
	cin >> turns;
	
	for (int i = 0; i < FIELD_ROW_COUNT; i++)
		cin >> world[ i ];
	
	// Read initial positions of the two runners.
	for(int i = 0; i < 2; i++)
		cin >> initRunnersx[i]>>initRunnersy[i];
	runnersx[0]=initRunnersx[0];
	runnersy[0]=initRunnersy[0];
	cin >> enemyCount;
	
	for(int i = 0; i < enemyCount; i++)
		cin >> initEnemiesx[i]>>initEnemiesy[i]>> program[ i ];
	enemiesx=initEnemiesx;
	enemiesy=initEnemiesy;
	
}

void generateMoves();

int main( int argc, char* argv[] ) 
{  
	init();	
	for ( int turnNumber = 0; turnNumber < turns; turnNumber++ ) {
		// Read the turn number (should be [ 0, turs ) unless we're late with a move.
		cin >> turn;

		// Read the current world description, since gold may be collected
		// and the floor may be destroyed.
		for(int i = 0; i < FIELD_ROW_COUNT; i++){
			cin >> world[ i ];
			for(int j=0;j<FIELD_COLUMN_COUNT;j++){
				enemyPresent[i][j]=-1;
			}
		}
		
		// Read the positions of each player's runners, current scores and
		// how long until each can dig.
		prevmyRow=runnersx[0];
		prevmyCol=runnersy[0];
		for(int i = 0; i < 2; i++){
			cin >> runnersx[i]>>runnersy[i] >> scores[ i ] >> digDelay[ i ];
		}
		prevEnemiesx=enemiesx;
		prevEnemiesy=enemiesy;
		
		for(int i=0; i < enemyCount; i++ ) {
			cin >> enemiesx[ i ]>>enemiesy[i] >> master[ i ];
			
			if(inRange(enemiesx[ i ],enemiesy[ i ])){
				if(world[enemiesx[ i ]][enemiesy[i]]=='-')
					world[enemiesx[ i ]][enemiesy[i]]='=';
				else
				enemyPresent[enemiesx[i]][enemiesy[i]]=i;	
			}
		}
		myRow = runnersx[0];
		myCol = runnersy[0];
		
		move="NONE";
		if(myRow >= 0 && myCol >= 0 ){
			// Remember when I visited this cell.
			visitTime[ myRow ][ myCol ] = turn;
			
			approachingLeft=false; 
			approachingRight = false;
			
			for(int k=0;k<4;k++){
				canMove[k]=false;
				threat[k]=false;
			}
			
			if(digDelay[0]==0)
				canDig=true;
			else
				canDig=false;
			
			canDigLeft = false;
			canDigRight = false;
			
			if(myRow> prevmyRow)
				myDirection = BOTTOM;
			else if(myRow < prevmyRow)
				myDirection = TOP;
			else if(myCol>prevmyCol)
				myDirection = RIGHT;
			else if(myCol<prevmyCol)
				myDirection = LEFT;
			
			for(int i=0; i < enemyCount; i++ ){ 	
				if(enemiesx[ i ]> prevEnemiesx[ i ] )
					enemyMoveDir[ i ] = BOTTOM;
				else if(enemiesx[ i ] < prevEnemiesx[ i ] )
					enemyMoveDir[ i ] = TOP;
				else if(enemiesy[ i ] > prevEnemiesy[ i ] )
					enemyMoveDir[ i ] = RIGHT;
				else if(enemiesy[ i ] < prevEnemiesy[ i ] )
					enemyMoveDir[ i ] = LEFT;
					
				if ( enemiesx[ i ] == myRow && enemiesy[ i ] < myCol && myCol - enemiesy[ i ] < THREAT_THRESHOLD ) {
					threat[ LEFT ] = true;
					if ( myCol - enemiesy[ i ] > 1 && enemyMoveDir[ i ] == RIGHT )
						approachingLeft = true;
				}
				
				if ( enemiesx[ i ] == myRow && enemiesy[ i ] > myCol && enemiesy[ i ] - myCol < THREAT_THRESHOLD ) {
					threat[ RIGHT ] = true;
					if ( enemiesy[ i ] - myCol > 1 && enemyMoveDir[ i ] == LEFT )
						approachingRight = true;
				}
				if ( enemiesy[ i ] == myCol && enemiesx[ i ] < myRow && myRow - enemiesx[ i ] <= THREAT_THRESHOLD )
					threat[ TOP ] = true;

				if ( enemiesy[ i ] == myCol && enemiesx[ i ] > myRow &&	enemiesx[ i ] - myRow <= THREAT_THRESHOLD )
					threat[ BOTTOM ] = true;
			}
			
			fallLeft=false;
			fallRight=false;
			
			if(myCol > 0 && world[ myRow ][ myCol - 1 ] != '=') {
				canMove[ LEFT ] = true;
				if(myRow + 1 < FIELD_ROW_COUNT ) {
					if ( world[ myRow ][ myCol - 1 ] != 'H' && world[ myRow + 1 ][ myCol - 1 ] == '=' &&
					   ( world[ myRow + 1 ][ myCol ] == '=' || world[ myRow + 1 ][ myCol ] == 'H' ) )
						canDigLeft = canDig;
				}
				if(myRow+2==FIELD_ROW_COUNT){
					if(world[myRow+1][myCol-1]=='-' )
						canMove[LEFT]=false;
				}
				if(world[myRow+1][myCol-1]=='.'){
					fallLeft=true;
				}
			}
			
			if(myCol + 1 < FIELD_COLUMN_COUNT && world[ myRow ][ myCol + 1 ] != '=' ) {
				canMove[ RIGHT ] = true;
				if(myRow + 1 < FIELD_ROW_COUNT ) {
					if ( world[ myRow ][ myCol + 1 ] != 'H' && world[ myRow + 1 ][ myCol + 1 ] == '=' &&
					   ( world[ myRow + 1 ][ myCol ] == '=' || world[ myRow + 1 ][ myCol ] == 'H' ) )
						canDigRight = canDig;
				}
				if(myRow+2==FIELD_ROW_COUNT){
					if(world[myRow+1][myCol+1]=='-' )
						canMove[RIGHT]=false;
					}
					if(world[myRow+1][myCol-1]=='.'){
						fallRight=true;
					}
				}
				if(myRow > 0 && world[ myRow ][ myCol ] == 'H' ) {
					canMove[ TOP ] = true;
			}
			if(myRow + 1 < FIELD_ROW_COUNT && world[ myRow + 1 ][ myCol ] != '=' ) {
				canMove[ BOTTOM ] = true;
			}
			if(myRow+1!=FIELD_ROW_COUNT)
				generateMoves();
		}
		cout<<move<<endl;
	}		
}
