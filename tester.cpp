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
		//cout<<qux[begin]<<" "<<quy[begin]<<endl;
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
	cout<<endl;
	if(r>0){
		int temp=goForGold[0];
		int p=temp;
		int d=0;
		
		while(temp){
			cout<<qux[temp]<<" "<<quy[temp]<<endl;
			p=temp;
			temp=parent[temp];
			d++;
		}
		sdistance=d;
		cout<<"P="<<p<<" "<<qux[p]<<" "<<quy[p]<<endl;
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

void generateMoves();

int main( int argc, char* argv[] ) 
{  
	
		// Read the current world description, since gold may be collected
		// and the floor may be destroyed.
		for(int i = 0; i < FIELD_ROW_COUNT; i++){
			cin >> world[ i ];
		}
		cout<<world[13][2]<<endl;
		int a,b;
		while(1){
			cin>>a>>b;
			shortestPath(a,b,'*',1);
		}
}
