//2023-03-14

#include "GMLS.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists)
{
	IloEnv Env_MP; // Init cplex environment
	IloModel Model_MP(Env_MP); // Init cplex model
	IloObjective Obj_MP(Env_MP); // Init obj
	IloRangeArray Cons_list(Env_MP);
	IloNumVarArray K_vars_list(Env_MP);
	IloNumVarArray V_vars_list(Env_MP);

	// solve the first core allocation MP
	SolveFirstMasterProblem(Values, Lists, Env_MP, Model_MP, Obj_MP, Cons_list, K_vars_list, V_vars_list);
	SolveSubProblem(Values, Lists); // solve the lot sizing SP

	while (1)
	{
		// add new column from previous SP to MP
		SolveUpdateMasterProblem(Values, Lists, Env_MP, Model_MP, Obj_MP, Cons_list, K_vars_list, V_vars_list);
		SolveSubProblem(Values, Lists);  // solve the lot sizing SP

		if (Values.core_find_flag == 1) // no more reduced cost from SP
		{
			printf("\n///////////////////////////////////////////////\n");
			printf("\n	A point in the core is find!\n");
			int machs_num = Values.machs_num;
			printf("\n	The point is ( ");
			for (int k = 0; k < machs_num; k++)
			{
				IloNum stable_cost = Lists.dual_prices_list[k];
				printf("%.2f ", stable_cost);
			}
			printf(")\n");
			printf("\n///////////////////////////////////////////////\n\n\n\n");
			break;
		}
	}

	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Cons_list.clear();
	Cons_list.end();
	K_vars_list.clear();
	K_vars_list.end();
	V_vars_list.clear();
	V_vars_list.end();

	// must end IloEnv as the last one
	Env_MP.removeAllProperties();
	Env_MP.end();
}