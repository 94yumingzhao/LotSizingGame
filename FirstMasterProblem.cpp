/*
2023-02-21
Core allocation master problem 
*/

#include "GMLS.h"
using namespace std;

void SolveFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP)
{
	int machs_num = Values.machs_num;

	/*************** cplex modeling by columns***************/
	/*
	// The initial matrix of the master problem
	vector<vector<int>> Master_Matrix_C;
	for (int k = 0; k < machs_num; k++)
	{
		vector<int> master_col;
		for (int p = 0; p < machs_num; p++)
		{
			if (k == p)
			{
				master_col.push_back(1); // one machine in each initial coalition
			}
			else
			{
				master_col.push_back(0);
			}
		}
		Master_Matrix_C.push_back(master_col);
	}

	// cplex column modeling
	IloNumArray  constraint_min(Env_MP); // cons left
	IloNumArray  constraint_max(Env_MP); // cons right

	// all constraints == 1
	for (int m = 0; m < machs_num; m++)
	{
		int coalition_cost = Lists.coalition_cost_list[m];
		constraint_min.add(IloNum(1)); // con >=1
		constraint_max.add(IloNum(IloInfinity));
	}

	IloRangeArray Cons_list= IloRangeArray(Env_MP, constraint_min, constraint_max); // init cons list
	Model_MP.add(Cons_list); // add cons list to the model

	constraint_min.end();
	constraint_max.end();
	
	Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // obj min & add obj

	IloNumVarArray w_vars(Env_MP); // init vars list
	for (int col = 0; col < machs_num; col++)
	{
		int obj_coeff = Lists.coalition_cost_list[col]; // obj coeff == c(S)
		IloNumColumn one_col = Obj_MP(obj_coeff); // init one column

		for (int row = 0; row < machs_num; row++)
		{
			one_col += Cons_list[row](Master_Matrix_C[col][row]); // update rows in this column
		}

		string w_name = "W_" + to_string(col + 1); 
		IloNumVar w_var(one_col, 0, IloInfinity, ILOFLOAT, w_name.c_str()); // init a var accoding to this column, var >=0
		w_vars.add(w_var); // add this var to vars list
		one_col.end(); // must end this column
	}
	*/

	/*************** cplex modeling by rows***************/

	// add the first row of grand coalition to the matrix
	vector<int> first_row;
	int cols_num = machs_num;
	int rows_num = machs_num;

	for (int col = 0; col < cols_num; col++)
	{
		first_row.push_back(1);
	}
	Lists.model_matrix_R.push_back(first_row);

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
		Lists.model_matrix_R.push_back(temp_row);
	}

	// set var V
	string v_name = "V";
	IloNum v_min = 0;
	IloNum v_max = IloInfinity;
	IloNumVar v_var = IloNumVar(Env_MP, v_min, v_max, ILOINT, v_name.c_str()); // V >=0

	// set var W
	IloNumVarArray w_vars(Env_MP);
	for (int m = 0; m < cols_num; m++)
	{
		string w_name = "W_" + to_string(m + 1);
		IloNum w_min = 0;
		IloNum w_max = IloInfinity;
		IloNumVar w_var = IloNumVar(Env_MP, w_min, w_max, ILOINT, w_name.c_str()); // W >=0
		w_vars.add(w_var);
	}

	// set obj
	IloExpr obj_sum(Env_MP);
	// obj  new model
	obj_sum = v_var;
	Obj_MP = IloMinimize(Env_MP, obj_sum); // obj MIN

	// obj  orginal model
	/*
	for (int m = 0; m < machs_num; m++)
	{
		obj_sum += w_vars[m]; // obj coeff == 1
	}
	Obj_MP = IloMaximize(Env_MP, obj_sum); // obj MAX
	*/
	
	Model_MP.add(Obj_MP); // add obj
	obj_sum.end();

	// set cons 
	for (int row = 0; row < rows_num+1; row++)
	{
		IloExpr con_sum(Env_MP);
		for (int col = 0; col < cols_num; col++)
		{		
			con_sum += Lists.model_matrix_R[row][col]* w_vars[col];
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

	

	printf("\n/////////// MP1 CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheFirstMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// MP1 CPLEX SOLVING END ////////////\n");

	printf("\n	////////// Status //////////\n\n");

	if (MP_flag == 0)
		printf("	The FIRST MP has NO feasible solutions\n");
	if (MP_flag == 1)
		printf("	The FIRST MP has feasible solutions\n");

	float Obj_value = Cplex_MP.getObjValue();
	printf("\n	The OBJECTIVE VALUE is %f\n", Obj_value);

	printf("\n	/////////// W /////////\n\n");

	for (int col = 0; col < cols_num; col++)
	{
		double w_soln_val = Cplex_MP.getValue(w_vars[col]);
		printf("	W_%d = %f\n", col +1, w_soln_val);
		Lists.master_solns_list.push_back(w_soln_val);
	}

	double v_soln_val = Cplex_MP.getValue(v_var);
	printf("	V = %f\n", v_soln_val);

	printf("\n	////////// Dual //////////\n\n");

	
	//for (int m = 0; m < machs_num; m++)
	//{
	//	float MP_dual_price;
	//	MP_dual_price = Cplex_MP.getDual((Cons_list)[m]);
	//	Lists.dual_prices_list.push_back(MP_dual_price);
	//	printf("Dual_%d = %f\n", m+1, MP_dual_price);
	//}
	

	Env_MP.end();

	cout << endl;
}


