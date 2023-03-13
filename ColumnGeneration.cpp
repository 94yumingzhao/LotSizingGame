//2023-03-12

#include "GMLS.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists)
{
	IloEnv Env_MP; // init cplex environment
	IloModel Model_MP(Env_MP); // init cplex model
	IloObjective Obj_MP(Env_MP); // init obj

	SolveFirstMasterProblem(Values, Lists, Env_MP, Model_MP, Obj_MP);
	SolveSubProblem(Values, Lists);

	while (1)
	{
		SolveUpdateMasterProblem(Values, Lists, Env_MP, Model_MP, Obj_MP);
		SolveSubProblem(Values, Lists);
	}
}