#include "Board.h"
#include <vector>
#include <iostream>
using namespace std;

#include "Side.h"

//Two vectors: One for the North board and one for the South board
Board::Board(int nHoles, int nInitialBeansPerHole)
	:m_nHoles(nHoles)
{
	//Set the pots
	m_beansPerHoleNorth.push_back(0);	
	m_beansPerHoleSouth.push_back(0);

	for (int i = 0; i < m_nHoles; i++)
	{
		m_beansPerHoleNorth.push_back(nInitialBeansPerHole);
		m_beansPerHoleSouth.push_back(nInitialBeansPerHole);
	}
}

Board::Board (const Board &other)
{
    m_nHoles = other.m_nHoles;
	
	for(int i = 0; i < other.m_beansPerHoleNorth.size();i++)
    {
        m_beansPerHoleNorth.push_back(other.m_beansPerHoleNorth[i]);
        m_beansPerHoleSouth.push_back(other.m_beansPerHoleSouth[i]);
    }
}

int Board::holes() const
{
	return m_nHoles;
}

int Board::beans(Side s, int hole) const
{
	if (hole < 0 || hole > m_nHoles)
		return -1;	
	if (s == NORTH)
		return m_beansPerHoleNorth[hole];
	return m_beansPerHoleSouth[hole];
}

int Board::beansInPlay(Side s) const
{
	int beansOnOneSide = 0;

	if (s == NORTH)
	{
		for (int i = 1; i < holes() + 1; i++)
		{
			beansOnOneSide += m_beansPerHoleNorth[i];
		}
	}

	if (s == SOUTH)
	{
		for (int i = 1; i < holes() + 1; i++)
		{
			beansOnOneSide += m_beansPerHoleSouth[i];
		}
	}
	return beansOnOneSide;	
}

int Board::totalBeans() const
{
	int total = 0;
	total = beansInPlay(NORTH) + beansInPlay(SOUTH) + beans(NORTH, 0) + beans(SOUTH, 0);
	return total;	
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
{
	if (hole <= 0 || hole > m_nHoles || beans(s, hole) == 0)	
		return false;

	//When we start off on the North side
	if (s == NORTH)
	{
		//Counter for the total number of beans to be deposited
		int beansNorth = beans(NORTH, hole);	
		m_beansPerHoleNorth[hole] = 0;
		int north = hole - 1;

		//Sow beans into each hole except the opponent's pot
		while (beansNorth > 0)	
		{
			//Sowing on the North side
			for ( ; north >= 0; north--)
			{
				m_beansPerHoleNorth[north]++;
				beansNorth--;

				if (beansNorth == 0)
				{
					endSide = NORTH;
					endHole = north;
					break;
				}
			}

			if (endSide == NORTH && endHole == north)
				break;

			north = m_nHoles;	//Reset the hole to the last hole if we return to the North side

			int trackSouth = -10;	//Initialized to an arbitrary value
			//Sowing on the South side
			for (int j = 1; j < m_nHoles + 1; j++)
			{
				m_beansPerHoleSouth[j]++;
				beansNorth--;

				if (beansNorth == 0)
				{
					endSide = SOUTH;
					endHole = j;
					trackSouth = j;
					break;
				}
			}

			if (endSide == SOUTH && endHole == trackSouth)
				break;
		}
	}

	else if (s == SOUTH)
	{
		//Counter for total number of beans to be deposited
		int beansSouth = beans(SOUTH, hole);
		m_beansPerHoleSouth[hole] = 0;
		int south = hole + 1;
		int first = 1;		//This is used to sow into the pot for the first time we run through

		//Sow beans into each hole except opponent's pot
		while (beansSouth > 0)	
		{
			//Sowing on the south side

			for ( ; south < m_nHoles + 1; south++)
			{
				m_beansPerHoleSouth[south]++;
				beansSouth--;

				if (beansSouth == 0)
				{
					//If we are at the South pot, break out of this loop
					if (south == m_nHoles + 1)		
						break;
					endSide = SOUTH;
					endHole = south;
					break;
				}
			}

			//Break out of the while loop if we end on a regular South hole
			if (endSide == SOUTH && endHole == south)
				break;

			//Fill in the pot when you sow for the first time
			if (first == 1)
			{
				m_beansPerHoleSouth[0]++;
				beansSouth--;
				first++;
				
				if (beansSouth == 0)
				{
					south = 0;
					endSide = SOUTH;
					endHole = south;
					break;
				}
			}

			south = 1;	//Reset the hole to the first hole if we return to the south side

			//This condition is invoked if we end on South's Pot
			if (beansSouth == 0)
			{
				endSide = SOUTH;
				endHole = south;
				break;
			}

			int northTracker = -20;	//Initialized to arbitrary value
			//Sowing on the north side
			for (int j = m_nHoles; j >= 1; j--)
			{		
				m_beansPerHoleNorth[j]++;
				beansSouth--;

				if (beansSouth == 0)
				{
					endSide = NORTH;
					endHole = j;
					northTracker = j;
					break;
				}
			}

			//Break out of the while loop if we end on the North side
			if (endSide == NORTH && endHole == northTracker)	
				break;
		}
	}

	return true;
}

bool Board::moveToPot(Side s, int hole, Side potOwner)
{
	if (hole <= 0 || hole > m_nHoles)
		return false;
	if (potOwner == NORTH && s == NORTH)
	{
		m_beansPerHoleNorth[0] += m_beansPerHoleNorth[hole];
		m_beansPerHoleNorth[hole] = 0;
	}

	else if (potOwner == NORTH && s == SOUTH)
	{
		m_beansPerHoleNorth[0] += m_beansPerHoleSouth[hole];
		m_beansPerHoleSouth[hole] = 0;
	}

	else if (potOwner == SOUTH && s == NORTH)
	{
		m_beansPerHoleSouth[0] += m_beansPerHoleNorth[hole];
		m_beansPerHoleNorth[hole] = 0;
	}

	else if (potOwner == SOUTH && s == SOUTH)
	{
		m_beansPerHoleSouth[0] += m_beansPerHoleSouth[hole];
		m_beansPerHoleSouth[hole] = 0;
	}

	return true;
}

bool Board::setBeans(Side s, int hole, int beans)
{
	if (hole < 0 || hole > m_nHoles || beans < 0)
		return false;	
	
	if (s == NORTH)
	{
		m_beansPerHoleNorth[hole] = beans;
		return true;
	}

	m_beansPerHoleSouth[hole] = beans;
	return true;
}
