
//========================================================================
// Timer t;                 // create a timer
// t.start();               // start the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

#if __cplusplus == 201103L  // C++11

#include <chrono>

class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {   
        std::chrono::duration<double,std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

#elif defined(_MSC_VER)  // not C++11, but Windows

#include <windows.h>

class Timer
{
  public:
    Timer()
    {
        QueryPerformanceFrequency(&ticksPerSecond);
        start();
    }
    void start()
    {
        QueryPerformanceCounter(&m_time);
    }
    double elapsed() const
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000.0 * (now.QuadPart - m_time.QuadPart)) / ticksPerSecond.QuadPart;
    }
  private:
    LARGE_INTEGER m_time;
    LARGE_INTEGER ticksPerSecond;
};

#else // not C++11 or Windows, so C++98

#include <ctime>

class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::clock();
    }-[-[
    double elapsed() const
    {   
        return (1000.0 * (std::clock() - m_time)) / CLOCKS_PER_SEC;
    }
  private:
    std::clock_t m_time;
};

#endif

Timer t;

#include "Player.h"
#include <string>
#include <iostream>
using namespace std;

#include "Board.h"
#include "Game.h"

//////////////////////////////////////////////////////////////	
//Player implementation
Player::Player(string name)
	:m_name(name)
{}

std::string Player::name() const
{
	return m_name;
}

bool Player::isInteractive() const
{
	return false;	
}

Player::~Player()
{}
////////////////////////////////////////////////////////////

//HumanPlayer Implementation
HumanPlayer::HumanPlayer(std::string name)
	:Player(name)
{}

bool HumanPlayer::isInteractive() const
{
	return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const
{
	//Pick your hole
	cout << "Select a hole, " << name() << ": ";
	int move = 0;

	//Keep reprompting until they enter a valid move
	while (true)
	{
		cin >> move;	//Can only enter integer values. No characters or strings
		
		if (move <= 0 || move > b.holes() )
		{
			cout << "The hole number must be from 1 to " << b.holes() << "." << endl;
			cout << "Select a hole, " << name() << ": ";
		}		

		else if (b.beans(s, move) == 0)
		{
			cout << "There are no beans in that hole." << endl;
			cout << "Select a hole, " << name() << ": "; 
		}

		else
			break;
	}

	return move;
}

HumanPlayer::~HumanPlayer()
{}

///////////////////////////////////////////////////////////

//BadPlayer Implementation
BadPlayer::BadPlayer(std::string name)
	:Player(name)
{}

//Returns the hole with the fewest beans on that side
int BadPlayer::chooseMove(const Board& b, Side s) const	
{
	//Initialize min and smallestHole
	int min = 0, smallestHole = 1;

	//Traverse through the vector and start off at first hole that contains at least one bean
	for (int x = 1; x < b.holes() + 1; x++)
	{
		if (b.beans(s, x) > 0)
		{
			min = b.beans(s,x);
			smallestHole = x;
			break;
		}
	}

	//Traverse through the vector and find the hole with the least beans AND contains at least one bean
	for (int i = smallestHole; i < b.holes() + 1; i++)
	{
		if (min > b.beans(s, i))
		{
			if (b.beans(s, i) < 1)
				continue;
			min = b.beans(s, i);
			smallestHole = i;
		}
	}

	//Invalid move if the Player sows an empty hole. This means the game is over
	if (min == 0)
	{
		cerr << "GAME OVER" << endl;
		return -1;
	}

	cout << name() << " chooses hole " << smallestHole << endl;
	return smallestHole;
}

BadPlayer::~BadPlayer()
{}
//////////////////////////////////////////////////////////

//SmartPlayer Implementation
SmartPlayer::SmartPlayer(std::string name)
	:Player(name)
{}

int SmartPlayer::minimax(const Board& b, Side curr, Side me, Side other, int& bestHole, bool finish) const
{
	int value = -1, endHole, v2;
	Side endSide;
	Board copy(b);

	//Make sure we haven't finished yet
	if (finish == false)
	{
		if (t.elapsed() > 4900)
		{
			value = b.beans(SOUTH, 0) - b.beans(NORTH, 0);
			return value;
		}

		//Return the best value if the game ends
		if (b.beansInPlay(curr) == 0 || b.beansInPlay(opponent(curr)) == 0)
		{
				//Move beans to appropriate pots if game is over
				if(copy.beansInPlay(curr) == 0)  	
				{
					for (int i = 0; i < copy.holes() + 1; i++)
						copy.moveToPot(opponent(curr), i , opponent(curr));
				}

				else if (copy.beansInPlay(opponent(curr)) == 0)
				{
					for (int j = 0; j < copy.holes() + 1; j++)
						copy.moveToPot(curr, j, curr);
				}

			//Heuristic is the greatest difference between the SmartPlayer's pot and opponent's pot
			value = b.beans(SOUTH, 0) - b.beans(NORTH, 0);
			return value;
		}

		for (int h = 1; h < b.holes() + 1; h++)
		{
			////We don't want SmartPlayer to pick moves on empty holes, so manually increment h
			if (b.beans(curr, h) == 0)
				continue;

			//Break out of the loop if our h iterator goes out of bounds
			if(h > b.holes())
				break;

			//Make a move on a copy of the board to see potential moves
			copy.sow(curr, h, endSide, endHole);

			//If we land in a pot, set curr to your opponent because minimax switches the order anyway 
			if(endSide == curr && endHole == 0)
			{
				curr = opponent(curr); 
			}

			//Capture results, if empty before and the hole opposite has a value, move all of them to Pot
			//Make sure we are not at a pot
			if (endSide == curr && endHole != 0)
			{
				if (copy.beans(curr, endHole) - 1 == 0 && copy.beans(opponent(curr), endHole) != 0)
				{
					copy.moveToPot(curr, endHole, curr);
					copy.moveToPot(opponent(curr), endHole, curr);
					copy.setBeans(curr, endHole, 0);
					copy.setBeans(opponent(curr), endHole, 0);
				}
			}

			//////////////////////////////////////////////////////////////
			v2 = minimax(copy, opponent(curr), me, other, bestHole, finish); 
		
			//Dont need to unmake move
				
			//South goes to positive infinity
			if (curr == SOUTH)
			{
				if (v2 > value)
				{
					bestHole = h;
					value = v2;
				}
			}

			//North goes to negative infinity
			else if (curr == NORTH)
			{
				if (v2 < value)
				{
					bestHole = h;
					v2 = value;
				}
			}
		}
		return value;
	}

	return -1;
}

int SmartPlayer::chooseMove(const Board& b, Side s) const	
{
	int bestHole = 0;

	Side me = s, other = opponent(s);
	bool finish = false;

	int tempHole = -1;
	int tempVal, value = MIN;
	
	//Start timer
	t.start();

	//Get the best hole by comparing values
	for (int i = 1; i < b.holes() + 1; i++)
	{
		tempVal = minimax(b, s, me, other, i, finish);
		if (tempVal > value)
		{
			value = tempVal;
			tempHole = i;
		}
	}

	bestHole = tempHole;

	//If bestHole happens to be empty, set it to an arbitrary hole so we can continue running
	if(b.beans(s, bestHole) == 0)
	{
		for(int i = 1; i < b.holes() + 1; i++)
		{
			if (b.beans(s, i) != 0)
			{
				bestHole = i;
				break;
			}
		}
	}

	cout << name() << " chooses hole " << bestHole << endl;
	return bestHole;
}

SmartPlayer::~SmartPlayer()
{}

