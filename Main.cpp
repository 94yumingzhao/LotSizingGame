/*
2023-02-13
lot sizing cooperative game
*/

#include "GMLS.h"
using namespace std;

int main()
{
	All_Values Values;
	All_Lists Lists;

	printf("\n\n+++++++++++++++++ Heuristics START +++++++++++++++++\n\n");

	for (int p=0; p < 7; p++)
	{
		FirstReadData(Values, Lists, p);
		SolveOriginalProblem(Values, Lists,p);
	}
	
	printf("\n\n+++++++++++++++++ Heuristics END +++++++++++++++++\n\n");

	NewReadData(Values, Lists, 10);
	ColumnGeneration(Values,Lists);

	system("pause");
	return 0;
}