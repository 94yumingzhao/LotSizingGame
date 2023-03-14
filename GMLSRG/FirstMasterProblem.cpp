/*
2023-02-21
Core allocation master problem 
*/

#include "GMLS.h"
using namespace std;

void SolveFirstMasterProblem(All_Values& Values,All_Lists& Lists)
{
	IloEnv Env_MP; // Init cplex environment
	IloModel Model_MP(Env_MP); // Init cplex model
	IloObjective Obj_MP(Env_MP); // Init obj

	int machs_num = Values.machs_num;

	/*************** cplex modeling by rows***************/

	// add the first row of grand coalition to the matrix
	vector<int> first_row;
	int cols_num = machs_num;
	int rows_num = machs_num;

	for (int col = 0; col < cols_num; col++)
	{
		first_row.push_back(1);
	}
	Lists.model_matrix.push_back(first_row);

	// add rows of single machine coalitions to the matrix
	for (int row = 0; row < rows_num; row++)
	{
		vector<int> temp_row;
		for (int col = 0; col < cols_num; col++)
		{
			if (row == col)
			{
				temp_row.push_back(1); // one machine in each initial coalition
			}
			else
			{
				temp_row.push_back(0);
			}
		}
		Lists.model_matrix.push_back(temp_row);
	}

	// set var V
	string v_name = "V";
	IloNum v_min = 0;
	IloNum v_max = IloInfinity;
	IloNumVar v_var = IloNumVar(Env_MP, v_min, v_max, ILOINT, v_name.c_str()); // V >=0

	// set var W
	IloNumVarArray W_vars_list(Env_MP);
	for (int m = 0; m < cols_num; m++)
	{
		string w_name = "W_" + to_string(m + 1);
		IloNum w_min = 0;
		IloNum w_max = IloInfinity;
		IloNumVar w_var = IloNumVar(Env_MP, w_min, w_max, ILOINT, w_name.c_str()); // W >=0
		W_vars_list.add(w_var);
	}

	// set obj
	IloExpr obj_sum(Env_MP);

	//obj_sum = v_var; 	// obj_A

	for (int col = 0; col < cols_num; col++) 	//obj_B
	{
		obj_sum += W_vars_list[col];
	}

	Obj_MP = IloMinimize(Env_MP, obj_sum); // obj MIN
	Model_MP.add(Obj_MP); // add obj
	obj_sum.end();

	// set cons 
	for (int row = 0; row < rows_num+1; row++)
	{
		IloExpr con_sum(Env_MP);
		for (int col = 0; col < cols_num; col++)
		{		
			con_sum += Lists.model_matrix[row][col]* W_vars_list[col];
		}
		if (row == 0)
		{
			Model_MP.add(con_sum == Lists.coalition_cost_list[row]); // con == c(N)
		}		
		if (row > 0)
		{
			//Model_MP.add(con_sum - v_var <= Lists.coalition_cost_list[row]); // con <= c(S) // con A
			Model_MP.add(con_sum <= Lists.coalition_cost_list[row]); // con <= c(S) // con B
		}
		con_sum.end();
	}


	printf("\n/////////// MP1 CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheFirstMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// MP1 CPLEX SOLVING END ////////////\n");

	printf("\n	////////// Status //////////\n\n");

	if (MP_flag == 0)
	{
		printf("	The FIRST MP has NO feasible solutions\n");
	}
	else
	{
		printf("	The FIRST MP has feasible solutions\n");

		int Obj_value = Cplex_MP.getObjValue();
		printf("\n	The OBJECTIVE VALUE is %d\n", Obj_value);

		printf("\n	/////////// W /////////\n\n");

		for (int col = 0; col < cols_num; col++)
		{
			int w_soln_val = Cplex_MP.getValue(W_vars_list[col]);
			printf("	W_%d = %d\n", col + 1, w_soln_val);
			Lists.MP_solns_list.push_back(w_soln_val);
		}

		// model B
		//int v_soln_val = Cplex_MP.getValue(v_var);
		//printf("	V = %d\n", v_soln_val);
	}

	
	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Cplex_MP.removeAllProperties();
	Cplex_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();

	// must end IloEnv object as the last one
	Env_MP.removeAllProperties();
	Env_MP.end();

	cout << endl;
}


