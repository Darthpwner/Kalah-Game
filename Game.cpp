#include "Game.h"
#include <iostream>
#include <vector>
using namespace std;
#include "Side.h"

Game::Game(const Board& b, Player* south, Player* north)
	:m_board(b)
{
	//Make north point to north player
	m_northPlayer = north;
	//Make south point to south player
	m_southPlayer = south;
	
	//Decide the player turn
	m_currSide = SOUTH;
}

void Game::display() const
{
	int playerTabCounter = m_board.holes()/2 + 1, accountForMoreThanOneDigit = 0;
	int northPotFactor = 0;
	if (m_board.beans(NORTH, 0) >= 10)
		northPotFactor++;

	for (int i = 0; i < playerTabCounter; i++)
		cout << "   ";
	for (int r = 0; r < northPotFactor; r++)
		cout << "   ";
	cout << m_northPlayer->name() << endl;

	for (int j = 1; j < m_board.holes() + 1; j++)
	{
		cout << "   " << m_board.beans(NORTH, j);
	}

	for (int s = 0; s < northPotFactor; s++)
		cout << "   ";
	cout << endl;

	cout << " " << m_board.beans(NORTH, 0);
	for (int k = 0; k < m_board.holes(); k++)
	{
			cout << "    ";	//extra spaces
	}

		for (int o = 1; o < m_board.holes() + 1; o++)
		{
			if (m_board.beans(NORTH, o) >= 10)
				accountForMoreThanOneDigit++;
		}

		for (int p = 1; p < m_board.holes() + 1; p++)
		{
			if (m_board.beans(SOUTH, p) >= 10)
				accountForMoreThanOneDigit++;
		}

	for (int q = 0; q < accountForMoreThanOneDigit; q++)
	{
		cout << " ";
	}

	cout << m_board.beans(SOUTH, 0);
	cout << endl;

	for (int l = 1; l < m_board.holes() + 1; l++)
	{
		cout << "   " << m_board.beans(SOUTH, l);
	}
	for (int t = 0; t < northPotFactor; t++)
		cout << "   ";
	cout << endl;
	for (int m = 0; m < playerTabCounter; m++)
		cout << "   ";
	for (int u = 0; u < northPotFactor; u++)
		cout << "   ";
	cout << m_southPlayer->name() << endl;
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const
{
	//If game is over
	if (m_board.beansInPlay(NORTH) == 0 || m_board.beansInPlay(SOUTH) == 0)
	{
		over = true;
		// if both pots have an equal number of beans, there is a tie
		if (m_board.beans(NORTH, 0) == m_board.beans(SOUTH, 0))
		{
			hasWinner = false;
		}
		//if one pot has more beans than the other, there is a winner
		else
		{
			if (m_board.beans(NORTH, 0) > m_board.beans(SOUTH, 0))
			{
				hasWinner = true;
				winner = NORTH;
			}

			else if (m_board.beans(SOUTH, 0) > m_board.beans(NORTH, 0))
			{
				hasWinner = true;
				winner = SOUTH;
			}
		}
	}

	else
		over = false;
}

bool Game::move()
{
	Side endSide;
	int endHole, hole;
	Player* playerTemp1;
	Player* playerTemp2;

	if (m_currSide == NORTH)
	{
		playerTemp1 = m_northPlayer;
		playerTemp2 = m_southPlayer;
	}

	else
	{
		playerTemp1 = m_southPlayer;
		playerTemp2 = m_northPlayer;
	}

	bool over, hasWinner;
	Side winner;

		//make a complete move
		hole = playerTemp1 -> chooseMove(m_board, m_currSide);	
		m_board.sow(m_currSide, hole, endSide, endHole); 
		
		//Move beans if game is over
		status(over, hasWinner, winner);
		if(over) 	
		{
			display();

			for (int i = 0; i < m_board.holes() + 1; i++)
				m_board.moveToPot(opponent(m_currSide), i , opponent(m_currSide));
			cout << "Sweeping remaining beans into " << playerTemp2->name() << "'s pot." << endl;
			return false;
		}

		//Keep taking another turn if the call to sow lands in the pot
		while (endSide == m_currSide && endHole == 0)
		{
			display();
			cout << playerTemp1 -> name() << " gets another turn." << endl;

			hole = playerTemp1 -> chooseMove(m_board, m_currSide);	
			m_board.sow(m_currSide, hole, endSide, endHole); 

			status(over, hasWinner, winner);
		
			//Move beans if game is over
			if(over) 	
			{
				display();

				for (int i = 0; i < m_board.holes() + 1; i++)
					m_board.moveToPot(opponent(m_currSide), i , opponent(m_currSide));
				cout << "Sweeping remaining beans into " << playerTemp2->name() << "'s pot." << endl;
				return false;
			}
		}
			
		//Capture results, if empty before and the hole opposite has a value, move all of them to Pot
		
		//Make sure we are not at a pot
		if (endSide == m_currSide && endHole != 0)
		{
			if (m_board.beans(m_currSide, endHole) - 1 == 0 && m_board.beans(opponent(m_currSide), endHole) != 0)
			{
				m_board.moveToPot(m_currSide, endHole, m_currSide);
				m_board.moveToPot(opponent(m_currSide), endHole, m_currSide);
				m_board.setBeans(m_currSide, endHole, 0);
				m_board.setBeans(opponent(m_currSide), endHole, 0);
			}
		}	

			status(over, hasWinner, winner);
			//if game is over, move all of the beans on the remaining side into the remaining side's pot and return false
			if(over) 	
			{
				display();

				for (int i = 0; i < m_board.holes() + 1; i++)
					m_board.moveToPot(opponent(m_currSide), i , opponent(m_currSide));
				cout << "Sweeping remaining beans into " << playerTemp2->name() << "'s pot." << endl;
				return false;
			}

	m_currSide = opponent(m_currSide);
	return true;
}

void Game::play()
{
	display();
	while (move())
	{
		//if neither player is human
		if (!m_northPlayer -> isInteractive() && !m_southPlayer ->isInteractive())
		{
			display();
			cout << "Press ENTER to continue";
			cin.ignore(10000, '\n');
		}

		else
			display();
	}

	display();

	bool over, hasWinner;
	Side winner;
	status(over, hasWinner, winner);
	if (hasWinner == false)
		cout << "The game resulted in a tie." << endl;
	if (winner == NORTH)
		cout << "The winner is " << m_northPlayer -> name() << "." << endl;
	else if (winner == SOUTH)
		cout << "The winner is " << m_southPlayer -> name() << "." << endl;

}

int Game::beans(Side s, int hole) const
{
	//If hole number is invalid, return -1
	if (hole < 0 || hole > m_board.holes())
		return -1;	
	return m_board.beans(s, hole);
}