// 2023-03-12

#include "GMLS.h"
using namespace std;

void SolveUpdateMasterProblem(All_Values& Values, All_Lists& Lists) {

	IloEnv Env_MP; // Init cplex environment
	IloModel Model_MP(Env_MP); // Init cplex model
	IloObjective Obj_MP(Env_MP); // Init obj

	int all_machs_num = Values.all_machs_num;

	// add new row to the matrix
	vector<int> new_row;
	int all_cols_num = all_machs_num;
	for (int col = 0; col < all_cols_num; col++) {
		int soln_val = Lists.SP_solns_list[col];
		new_row.push_back(soln_val);
	}
	Lists.model_matrix.push_back(new_row);

	// set var V
	string v_name = "V";
	IloNum v_min = 0;
	IloNum v_max = IloInfinity;
	IloNumVar v_var = IloNumVar(Env_MP, 0, IloInfinity, ILOINT, v_name.c_str()); // V >=0

	// set var W
	IloNumVarArray W_Vars(Env_MP);
	for (int m = 0; m < all_machs_num; m++) {
		cout << m << endl;
		string w_name = "W_" + to_string(m + 1);
		IloNum w_min = 0;
		IloNum w_max = IloInfinity;

		IloNumVar W_Var = IloNumVar(Env_MP, w_min, w_max, ILOINT, w_name.c_str()); // W >=0
		W_Vars.add(W_Var);
	}


	IloExpr sum_obj(Env_MP);

	// sum_obj = v_var; // model A

	for (int col = 0; col < all_cols_num; col++) 	// model B
	{
		sum_obj += W_Vars[col];
	}

	Obj_MP = IloMinimize(Env_MP, sum_obj); // obj MIN
	Model_MP.add(Obj_MP); // add obj
	sum_obj.end();

	// set cons
	int all_rows_num = Lists.model_matrix.size();
	for (int row = 0; row < all_rows_num; row++) {
		IloExpr sum_1(Env_MP);
		for (int col = 0; col < all_cols_num; col++) {
			sum_1 += Lists.model_matrix[row][col] * W_Vars[col];
		}
		if (row == 0) {
			Model_MP.add(sum_1 == Lists.coalition_cost_list[row]); // con == c(N)
		}
		if (row > 0) {
			// Model_MP.add(sum_1 - v_var <= Lists.coalition_cost_list[row]); // con <= c(S) // model A
			Model_MP.add(sum_1 <= Lists.coalition_cost_list[row]); // con <= c(S) // model B
		}
		sum_1.end();
	}

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheNewMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// CPLEX SOLVING END ////////////\n");


	if (MP_flag == 0) {
		printf("\n\t This MP has NO feasible solutions\n");
	}
	else {
		printf("\n\t This MP has feasible solutions\n");

		int Obj_value = Cplex_MP.getObjValue();
		printf("\n\t The OBJECTIVE VALUE is %d\n", Obj_value);

		printf("\n	//////////W//////////\n\n");

		Lists.MP_solns_list.clear();
		for (int col = 0; col < all_cols_num; col++) {
			int w_soln_val = Cplex_MP.getValue(W_Vars[col]);
			printf("\t W_%d = %d\n", col + 1, w_soln_val);
			Lists.MP_solns_list.push_back(w_soln_val);
		}

		// model A
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
