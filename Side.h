#ifndef SIDE_H
#define SIDE_H

enum Side { NORTH, SOUTH };	

const int MAX = 10000;
const int MIN = -10000;

inline
	Side opponent(Side s)
	{
	    return Side(1 - s);
	}

#endif //SIDE_H

