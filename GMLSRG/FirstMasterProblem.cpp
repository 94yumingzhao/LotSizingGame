/*
2023-02-21
Core allocation master problem
*/

#include "GMLS.h"
using namespace std;

void SolveFirstMasterProblem(All_Values& Values, All_Lists& Lists) {

	IloEnv Env_MP; // Init cplex environment
	IloModel Model_MP(Env_MP); // Init cplex model
	IloObjective Obj_MP(Env_MP); // Init obj
	IloNumVarArray W_Vars(Env_MP);

	int M_num = Values.M_num;

	// add the first row of grand coalition to the matrix
	vector<int> first_row;
	int all_cols_num = M_num;
	int all_rows_num = M_num;

	for (int col = 0; col < all_cols_num; col++) {
		first_row.push_back(1);
	}
	Lists.model_matrix.push_back(first_row);

	// add rows of single machine coalitions to the matrix
	for (int row = 0; row < all_rows_num; row++) {
		vector<int> temp_row;
		for (int col = 0; col < all_cols_num; col++) {
			if (row == col) {
				temp_row.push_back(1); // one machine in each initial coalition
			}
			else {
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
	for (int m = 0; m < all_cols_num; m++) {
		string W_name = "W_" + to_string(m + 1);
		IloNum var_min = 0;
		IloNum var_max = IloInfinity;
		IloNumVar W_Var = IloNumVar(Env_MP, var_min, var_max, ILOINT, W_name.c_str()); // W >=0
		W_Vars.add(W_Var);
	}

	// set obj
	IloExpr sum_obj(Env_MP);//obj_B
	for (int col = 0; col < all_cols_num; col++) {
		sum_obj += W_Vars[col];
	}

	Obj_MP = IloMinimize(Env_MP, sum_obj); // obj MIN
	Model_MP.add(Obj_MP); // add obj
	sum_obj.end();

	// set cons 
	for (int row = 0; row < all_rows_num + 1; row++) {
		IloExpr sum_left(Env_MP);
		for (int col = 0; col < all_cols_num; col++) {
			sum_left += Lists.model_matrix[row][col] * W_Vars[col];
		}
		if (row == 0) {
			Model_MP.add(sum_left == Lists.coalition_cost_list[row]); // con == c(N)
		}
		if (row > 0) {
			//Model_MP.add(sum_1 - v_var <= Lists.coalition_cost_list[row]); // con <= c(S) // con A
			Model_MP.add(sum_left <= Lists.coalition_cost_list[row]); // con <= c(S) // con B
		}
		sum_left.end();
	}

	printf("\n/////////// MP1 CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheFirstMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// MP1 CPLEX SOLVING END ////////////\n");

	printf("\n\t ////////// Status //////////\n\n");
	if (MP_flag == 0) {
		printf("\t The FIRST MP has NO feasible solutions\n");
	}
	else {
		printf("\t The FIRST MP has feasible solutions\n");
		printf("\n\tThe OBJECTIVE VALUE is %f\n", Cplex_MP.getObjValue());
		printf("\n\t /////////// W /////////\n\n");

		for (int col = 0; col < all_cols_num; col++) {
			double w_soln_val = Cplex_MP.getValue(W_Vars[col]);
			printf("\t W_%d = %f\n", col + 1, w_soln_val);
			Lists.MP_solns_list.push_back(w_soln_val);
		}
	}

	Cplex_MP.removeAllProperties();
	Cplex_MP.end();
	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();

	cout << endl;
}


