//2023-03-14

#include "GMLS.h"
using namespace std;

void SolveUpdateMasterProblem(
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

	// add new row to the matrix
	vector<int> new_col;
	int rows_num = machs_num;
	for (int row = 0; row < rows_num; row++)
	{
		int soln_val = Lists.SP_solns_list[row];
		new_col.push_back(soln_val);
	}


	int cltn_cost = 0; // coalition cost of the new col's pattern
	int cltn_pats_num = Lists.coalitions_list.size();
	
	// find the coalition cost of this new col's pattern
	for (int pat = 0; pat < cltn_pats_num; pat++)
	{
		int same_pat_flag = -1;
		for (int row = 0; row < rows_num; row++)
		{
			if (new_col[row]!=
				Lists.coalitions_list[pat].pattern[row])
			{
				same_pat_flag = 0;
				cout << endl;
			}
		}
		if (same_pat_flag == -1) // the new col's pattern == this coalition's pattern
		{
			cltn_cost = Lists.coalitions_list[pat].cost;
			cout << endl;
			break;
		}
	}

	Lists.model_matrix.push_back(new_col);
	int cols_num = Lists.model_matrix.size();

	IloNum obj_para = cltn_cost; // obj coeff == c(N)
	IloNumColumn CplexCol = Obj_MP(obj_para); // Init one column

	for (int row = 0; row < rows_num; row++)
	{
		IloNum para_val = new_col[row];
		CplexCol += Cons_list[row](para_val); // update rows in this column

		string k_name = "K_" + to_string(cols_num + 1);
		IloNum k_min = 0;
		IloNum k_max = IloInfinity;
		IloNumVar k_var(CplexCol, k_min, k_max, ILOFLOAT, k_name.c_str()); // Init a var accoding to this column, var >=0
		K_vars_list.add(k_var); // add this var to vars_list list
	}

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
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

		Lists.MP_solns_list.clear();
		for (int col = 0; col < cols_num; col++)
		{
			int k_soln_val = Cplex_MP.getValue(K_vars_list[col]);
			printf("	k_%d = %d\n", col + 1, k_soln_val);
			Lists.MP_solns_list.push_back(k_soln_val);
		}

		Lists.dual_prices_list.clear();
		for (int row = 0; row < rows_num; row++)
		{
			IloNum MP_dual_price = Cplex_MP.getDual((Cons_list)[row]);
			Lists.dual_prices_list.push_back(MP_dual_price);
			printf("\n	Dual_%d = %f", row + 1, MP_dual_price);
		}
		//int v_soln_val = Cplex_MP.getValue(V_vars_list[0]);
		//printf("	V = %d\n", v_soln_val);
	}

	Cplex_MP.removeAllProperties();
	Cplex_MP.end();

	cout << endl;
}
