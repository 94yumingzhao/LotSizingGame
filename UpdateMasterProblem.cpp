// 2023-03-12

#include "GMLS.h"
using namespace std;

void SolveUpdateMasterProblem(All_Values& Values,All_Lists& Lists)
{
	IloEnv Env_MP; // init cplex environment
	IloModel Model_MP(Env_MP); // init cplex model
	IloObjective Obj_MP(Env_MP); // init obj

	int machs_num = Values.machs_num;

	// add new row to the matrix
	vector<int> new_row;
	int cols_num = machs_num;
	for (int col = 0; col < cols_num; col++)
	{
		int soln_val = Lists.coalition_solns_list[col];
		new_row.push_back(soln_val);
	}
	Lists.model_matrix_R.push_back(new_row);

	// set var V
	string v_name = "V";
	IloNum v_min = 0;
	IloNum v_max = IloInfinity;
	IloNumVar v_var = IloNumVar(Env_MP, 0, IloInfinity, ILOINT, v_name.c_str()); // V >=0

	// set var W
	IloNumVarArray w_vars(Env_MP);
	for (int m = 0; m < machs_num; m++)
	{
		cout << m << endl;
		string w_name = "W_" + to_string(m + 1);
		IloNum w_min = 0;
		IloNum w_max = IloInfinity;

		IloNumVar w_var = IloNumVar(Env_MP, w_min, w_max, ILOINT, w_name.c_str()); // W >=0
		w_vars.add(w_var);
	}

	// set obj
	IloExpr obj_sum(Env_MP);
	obj_sum = v_var;
	Obj_MP = IloMinimize(Env_MP, obj_sum); // obj MIN
	Model_MP.add(Obj_MP); // add obj
	obj_sum.end();

	// set cons
	int rows_num = Lists.model_matrix_R.size();
	for (int row = 0; row < rows_num; row++)
	{
		IloExpr con_sum(Env_MP);
		for (int col = 0; col < cols_num; col++)
		{
			con_sum += Lists.model_matrix_R[row][col] * w_vars[col];
		}
		if (row == 0)
		{
			Model_MP.add(con_sum == Lists.coalition_cost_list[row]); // con == c(N)
		}
		if (row > 0)
		{
			Model_MP.add(con_sum - v_var <= Lists.coalition_cost_list[row]); // con <= c(S)
		}
		con_sum.end();
	}

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheNewMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// CPLEX SOLVING END ////////////\n");


	if (MP_flag == 0)
	{
		printf("\n	This MP has NO feasible solutions\n");
	}
	else
	{
		printf("\n	This MP has feasible solutions\n");

		int Obj_value = Cplex_MP.getObjValue();
		printf("\n	The OBJECTIVE VALUE is %d\n", Obj_value);

		printf("\n	//////////W//////////\n\n");

		Lists.master_solns_list.clear();
		for (int col = 0; col < cols_num; col++)
		{
			int w_soln_val = Cplex_MP.getValue(w_vars[col]);
			printf("	W_%d = %d\n", col + 1, w_soln_val);
			Lists.master_solns_list.push_back(w_soln_val);
		}

		int v_soln_val = Cplex_MP.getValue(v_var);
		printf("	V = %d\n", v_soln_val);

	}

	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();

	cout << endl;
}
