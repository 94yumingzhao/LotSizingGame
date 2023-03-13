//2023-03-12

#include "GMLS.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists)
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
				printf("%d ", Lists.master_solns_list[k]);
			}
			printf(")\n");
			printf("\n///////////////////////////////////////////////\n\n\n\n");
			break;
		}
	}
}