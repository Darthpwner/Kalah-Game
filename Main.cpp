#include "Board.h"
#include "Game.h"
#include "Player.h"
#include "Side.h"
#include <string>
//include guards
#include <iostream>
using namespace std;

int main()
	{
			HumanPlayer thien("Matthew");
			HumanPlayer matthew("Travis");
        	HumanPlayer hp("Ashvin");
        	BadPlayer bp("Matthew");
			SmartPlayer sp("ARKO");
			SmartPlayer sp1("MATTHEW");
			HumanPlayer dp ("Donald");

			HumanPlayer kevin ("Kevin");
			SmartPlayer s1("Smart1");
			SmartPlayer s2("Smart2");
			BadPlayer bad("Bad");


        	//Board b(5,2);
			Board b(6, 4);
			Game g(b, &s2, &thien);
			g.play();
	}
