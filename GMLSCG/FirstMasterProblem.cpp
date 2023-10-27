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
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	IloNumVarArray& V_Vars) {

	int M_num = Values.M_num;
	int all_cols_num = M_num + 1;
	int all_rows_num = M_num;

	// The initial matrix of the master problem
	vector<int> first_col;
	for (int row = 0; row < all_rows_num; row++) {
		first_col.push_back(1);
	}
	Lists.model_matrix.push_back(first_col);

	for (int col = 0; col < all_cols_num - 1; col++) {
		vector<int> temp_col;
		for (int row = 0; row < all_rows_num; row++) {
			if (col == row) {
				temp_col.push_back(1); // one machine in each initial coalition
			}
			else {
				temp_col.push_back(0);
			}
		}
		Lists.model_matrix.push_back(temp_col);
	}

	// cplex column modeling
	IloNumArray  con_min(Env_MP); // cons left
	IloNumArray  con_max(Env_MP); // cons right

	for (int m = 0; m < all_rows_num; m++) { // con >=1
		con_min.add(IloNum(1)); // con LB
		con_max.add(IloNum(IloInfinity)); // con UB
	}

	Cons_MP = IloRangeArray(Env_MP, con_min, con_max); // Init cons list
	Model_MP.add(Cons_MP); // add cons list to the model

	con_min.end();
	con_max.end();

	Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // obj min & add obj

	for (int col = 0; col < all_cols_num; col++) {
		IloNum obj_para = Lists.coalitions_list[col].cost; // obj para == c(S)
		IloNumColumn CplexCol = Obj_MP(obj_para); // Init one column
		for (int row = 0; row < all_rows_num; row++) {
			IloNum row_para = Lists.model_matrix[col][row];
			CplexCol += Cons_MP[row](row_para); // update rows in this column
		}

		string K_name = "K_" + to_string(col + 1);
		IloNum var_min = 0;
		IloNum var_max = IloInfinity;
		IloNumVar K_Var(CplexCol, var_min, var_max, ILOFLOAT, K_name.c_str()); // Init a var accoding to this column, var >=0
		Vars_MP.add(K_Var); // add this var to vars_list list

		CplexCol.end(); // must end this IloNumColumn object
	}

	printf("\n/////////// MP1 CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_MP(Env_MP); // Init cplex solver
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel("TheFirstMasterProblemR.lp");
	bool MP_flag = Cplex_MP.solve(); // solve the cplex model
	printf("\n/////////// MP1 CPLEX SOLVING END ////////////\n");

	printf("\n	////////// Status //////////\n\n");

	if (MP_flag == 0) {
		printf("\t The FIRST MP has NO feasible solutions\n");
	}
	else {
		printf("\t The FIRST MP has feasible solutions\n");
		printf("\n\t The OBJECTIVE VALUE is %f\n", Cplex_MP.getObjValue());
		printf("\n\t /////////// W /////////\n\n");

		for (int col = 0; col < all_cols_num; col++) {
			double soln_val = Cplex_MP.getValue(Vars_MP[col]);
			Lists.MP_solns_list.push_back(int(soln_val));
			printf("\t k_%d = %f\n", col + 1, soln_val);
		}
	}

	Lists.dual_prices_list.clear();

	for (int row = 0; row < all_rows_num; row++) {
		IloNum dual_val = Cplex_MP.getDual((Cons_MP)[row]);
		Lists.dual_prices_list.push_back(dual_val);
		printf("\n\t Dual_%d = %f", row + 1, dual_val);
	}

	Cplex_MP.removeAllProperties();
	Cplex_MP.end();

	cout << endl;
}


