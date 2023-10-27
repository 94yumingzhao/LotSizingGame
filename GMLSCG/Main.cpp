/*
//2023-03-14
lot sizing cooperative game
*/

#include "GMLS.h"
using namespace std;

int main() {
	All_Values Values;
	All_Lists Lists;

	printf("\n\n+++++++++++++++++ Heuristics START +++++++++++++++++\n\n");

	// number of all possible coalition patterns = 2^players_num-1
	int coalitions_pattern_num = 1;
	int players_num = Values.M_num;
	for (int k = 0; k < players_num; k++) {
		coalitions_pattern_num = coalitions_pattern_num * 2;
	}
	coalitions_pattern_num = coalitions_pattern_num - 1;

	// read data and solve to get the coalition costs of all possible coalition patterns
	for (int k = 0; k < coalitions_pattern_num; k++) {
		FirstReadData(Values, Lists, k);
		SolveOriginalProblem(Values, Lists, k);
	}

	printf("\n\n+++++++++++++++++ Heuristics END +++++++++++++++++\n\n");

	NewReadData(Values, Lists, 10); // read data again for CG loop
	ColumnGeneration(Values, Lists); // find a stable point in core with CG loop 

	system("pause");
	return 0;
}