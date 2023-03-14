/*
//2023-03-14
Core allocation master problem
*/

#include "GMLS.h"
using namespace std;

void SolveFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_list,
	IloNumVarArray& K_vars_list,
	IloNumVarArray& V_vars_list)
{

	int machs_num = Values.machs_num;
	int cols_num = machs_num + 1;
	int rows_num = machs_num;

	/*************** cplex modeling by columns***************/

	// The initial matrix of the master problem
	vector<int> first_col;
	for (int row = 0; row < rows_num; row++)
	{
		first_col.push_back(1);
	}
	Lists.model_matrix.push_back(first_col);

	for (int col = 0; col < cols_num - 1; col++)
	{
		vector<int> temp_col;
		for (int row = 0; row < rows_num; row++)
		{
			if (col == row)
			{
				temp_col.push_back(1); // one machine in each initial coalition
			}
			else
			{
				temp_col.push_back(0);
			}
		}
		Lists.model_matrix.push_back(temp_col);
	}

	// cplex column modeling
	IloNumArray  con_min(Env_MP); // cons left
	IloNumArray  con_max(Env_MP); // cons right

	// all constraints == 1
	for (int m = 0; m < rows_num; m++)
	{
		// con >=1
		con_min.add(IloNum(1)); // con LB
		con_max.add(IloNum(IloInfinity)); // con UB
	}

	Cons_list = IloRangeArray(Env_MP, con_min, con_max); // Init cons list
	Model_MP.add(Cons_list); // add cons list to the model

	con_min.end();
	con_max.end();

	Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // obj min & add obj

	for (int col = 0; col < cols_num; col++)
	{
		IloNum obj_para = Lists.coalitions_list[col].cost; // obj coeff == c(S)
		IloNumColumn CplexCol = Obj_MP(obj_para); // Init one column

		for (int row = 0; row < rows_num; row++)
		{
			IloNum matrix_para = Lists.model_matrix[col][row];
			CplexCol += Cons_list[row](matrix_para); // update rows in this column
		}

		string k_name = "K_" + to_string(col + 1);
		IloNum k_min = 0;
		IloNum k_max = IloInfinity;

		IloNumVar k_var(CplexCol, k_min, k_max, ILOFLOAT, k_name.c_str()); // Init a var accoding to this column, var >=0
		K_vars_list.add(k_var); // add this var to vars_list list

		CplexCol.end(); // must end this IloNumColumn object
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
			int k_soln_val = Cplex_MP.getValue(K_vars_list[col]);
			printf("	k_%d = %d\n", col + 1, k_soln_val);
			Lists.MP_solns_list.push_back(k_soln_val);
		}
	}

	Lists.dual_prices_list.clear();
	for (int row = 0; row < rows_num; row++)
	{
		IloNum MP_dual_price = Cplex_MP.getDual((Cons_list)[row]);
		Lists.dual_prices_list.push_back(MP_dual_price);
		printf("\n	Dual_%d = %f", row + 1, MP_dual_price);
	}

	Cplex_MP.removeAllProperties();
	Cplex_MP.end();

	cout << endl;
}


