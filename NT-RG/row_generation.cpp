/*
2023-10-30
*/

#include "T.h"
using namespace std;

void RowGeneration(All_Values& Values, All_Lists& Lists) {
	SolveFirstMasterProblem(Values, Lists);
	SolveSubProblem(Values, Lists);

	while (1) {
		SolveUpdateMasterProblem(Values, Lists);
		SolveSubProblem(Values, Lists);

		if (Values.core_find_flag == 1) {
			printf("\n///////////////////////////////////////////////\n");
			printf("\n\t A point in the core is find!\n");
			int M_num = Values.M_num;
			printf("\n\t The point is ( ");
			for (int k = 0; k < M_num; k++) {
				IloNum stable_cost = Lists.MP_solns_list[k];
				printf("\t %.2f ", stable_cost);
			}
			printf("\n");
			printf("\n///////////////////////////////////////////////\n\n\n\n");
			break;
		}
	}
}