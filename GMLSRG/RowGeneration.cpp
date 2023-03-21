//2023-03-12

#include "GMLS.h"
using namespace std;

void RowGeneration(All_Values& Values, All_Lists& Lists)
{
	SolveFirstMasterProblem(Values, Lists);
	SolveSubProblem(Values, Lists);

	while (1)
	{
		SolveUpdateMasterProblem(Values, Lists);
		SolveSubProblem(Values, Lists);

		if (Values.core_find_flag == 1)
		{
			printf("\n///////////////////////////////////////////////\n");
			printf("\n	A point in the core is find!\n");
			int machs_num = Values.machs_num;
			printf("\n	The point is ( ");
			for (int k = 0; k < machs_num; k++)
			{
				IloNum stable_cost = Lists.MP_solns_list[k];
				printf("%.2f ", stable_cost);
			}
			printf(")\n");
			printf("\n///////////////////////////////////////////////\n\n\n\n");
			break;
		}
	}
}