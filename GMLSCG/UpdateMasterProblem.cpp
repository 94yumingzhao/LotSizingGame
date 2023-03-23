//2023-03-14

#include "GMLS.h"
using namespace std;

void SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	IloNumVarArray& V_Vars) {

	int all_machs_num = Values.all_machs_num;

	// add new row to the matrix
	vector<int> new_col;
	int all_rows_num = all_machs_num;
	for (int row = 0; row < all_rows_num; row++) {
		int soln_val = Lists.SP_solns_list[row];
		new_col.push_back(soln_val);
	}

	int cltn_cost = 0; // coalition cost of the new col's pattern
	int all_cltn_patterns_num = Lists.coalitions_list.size();

	// find the coalition cost of this new col's pattern
	for (int pat = 0; pat < all_cltn_patterns_num; pat++) {
		int same_pat_flag = -1;
		for (int row = 0; row < all_rows_num; row++) {
			if (new_col[row] !=
				Lists.coalitions_list[pat].pattern[row]) {
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
	int all_cols_num = Lists.model_matrix.size();

	IloNum obj_para = cltn_cost; // obj para == c(N)
	IloNumColumn CplexCol = Obj_MP(obj_para); // Init one column
	for (int row = 0; row < all_rows_num; row++) {
		IloNum row_para = new_col[row];
		CplexCol += Cons_MP[row](row_para); // update rows in this column

		string K_name = "K_" + to_string(all_cols_num + 1);
		IloNum var_min = 0;
		IloNum var_max = IloInfinity;
		IloNumVar K_Var(CplexCol, var_min, var_max, ILOFLOAT, K_name.c_str()); // Init a var accoding to this column, var >=0
		Vars_MP.add(K_Var); // add this var to vars_list list
	}

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheNewMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// CPLEX SOLVING END ////////////\n");

	if (MP_flag == 0) {
		printf("\n	This MP has NO feasible solutions\n");
	}
	else {
		printf("\n	This MP has feasible solutions\n");
		printf("\n	The OBJECTIVE VALUE is %f\n", Cplex_MP.getObjValue());
		printf("\n	//////////W//////////\n\n");

		Lists.MP_solns_list.clear();
		for (int col = 0; col < all_cols_num; col++) {
			int soln_val = Cplex_MP.getValue(Vars_MP[col]);
			Lists.MP_solns_list.push_back(soln_val);
			printf("\t k_%d = %d\n", col + 1, soln_val);
		}

		Lists.dual_prices_list.clear();

		for (int row = 0; row < all_rows_num; row++) {
			IloNum dual_val = Cplex_MP.getDual((Cons_MP)[row]);
			Lists.dual_prices_list.push_back(dual_val);
			printf("\n\t Dual_%d = %f", row + 1, dual_val);
		}
	}

	Cplex_MP.removeAllProperties();
	Cplex_MP.end();

	cout << endl;
}
