// 2023-02-21 lot sizing sub problem


#include "GMLS.h"
using namespace std;

void SolveSubProblem(All_Values& Values, All_Lists& Lists) {
	int T_num = Values.T_num;
	int M_num = Values.M_num;

	IloEnv Env_SP;
	IloModel Model_SP(Env_SP);
	IloCplex Cplex_SP(Env_SP);

	// vars
	IloArray<IloNumVar> X_vars_list(Env_SP);
	IloArray<IloNumVar> Y_vars_list(Env_SP);
	IloArray<IloNumVar> I_vars_list(Env_SP);
	IloArray<IloNumVar> Z_vars_list(Env_SP);

	for (int t = 0; t < T_num; t++) {
		string X_name = "X_" + to_string(t + 1);
		string Y_name = "Y_" + to_string(t + 1);
		string I_name = "I_" + to_string(t + 1);

		IloNumVar X_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, X_name.c_str());
		IloNumVar Y_var = IloNumVar(Env_SP, 0, 1, ILOINT, Y_name.c_str());
		IloNumVar I_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, I_name.c_str());

		X_vars_list.add(X_var);
		Y_vars_list.add(Y_var);
		I_vars_list.add(I_var);
	}

	for (int m = 0; m < M_num; m++) {
		string Z_name = "Z_" + to_string(m + 1);
		Z_vars_list.add(IloNumVar(Env_SP, 0, 1, ILOINT, Z_name.c_str()));
	}

	IloArray<IloArray<IloArray<IloNumVar>>> K_Vars(Env_SP); // Z(m,n,i,t)
	for (int m = 0; m < M_num; m++) {
		K_Vars.add(IloArray < IloArray<IloArray<IloNumVar>>>(Env_SP));
		for (int n = 0; n < M_num; n++) {
			K_Vars[m].add(IloArray<IloArray<IloNumVar>>(Env_SP));
			for (int t = 0; t < T_num; t++) {
				string var_name = "K_" + to_string(m + 1) + "_" + to_string(n + 1)  + "_" + to_string(t + 1);
				IloNumVar K_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, var_name.c_str());
				K_Vars[m][n].add(K_var);
			}
		}
	}

	// obj
	IloExpr sum_obj(Env_SP);
	for (int t = 0; t < T_num; t++) {
		sum_obj -= Lists.primal_parameters[t].c_X * X_vars_list[t];
	}

	for (int t = 0; t < T_num; t++) {
		sum_obj -= Lists.primal_parameters[t].c_Y * Y_vars_list[t];
	}

	for (int t = 0; t < T_num; t++) {
		sum_obj -= Lists.primal_parameters[t].c_I * I_vars_list[t];
	}

	for (int m = 0; m < M_num; m++) {
		sum_obj += Lists.MP_solns_list[m] * Z_vars_list[m]; // sum W_m * Z_m
	}

	IloObjective Obj_SP = IloMaximize(Env_SP, sum_obj); // obj max
	Model_SP.add(Obj_SP);
	sum_obj.end();

	// number of con 1 == T
	for (int t = 0; t < T_num; t++) {
		IloExpr sum_1(Env_SP);

		for (int m = 0; m < M_num; m++) {
			sum_1 += Lists.demand_matrix[m][t] * Z_vars_list[m]; // sum d_mt *  Z_m
		}

		if (t == 0) {
			Model_SP.add(X_vars_list[t] == sum_1 + I_vars_list[t]);
		}

		if (t > 0) {
			Model_SP.add(X_vars_list[t] + I_vars_list[t - 1] == sum_1 + I_vars_list[t]);
		}

		sum_1.end();
	}

	// number of con 2 == T 
	for (int t = 0; t < T_num; t++) {
		Model_SP.add(X_vars_list[t] <= Values.machine_capacity * Y_vars_list[t]);
	}

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	Cplex_SP.extract(Model_SP);
	Cplex_SP.exportModel("SubProblem1.lp");
	bool SP_flag = Cplex_SP.solve();
	printf("\n/////////// CPLEX SOLVING END ////////////\n");

	if (SP_flag == 0) {
		printf("\n\t This SP has NO FEASIBLE solns\n");
	}
	else {
		printf("\n\t	This SP has FEASIBLE solns\n");

		int obj_val = Cplex_SP.getObjValue();

		if (obj_val <= 0) {
			Values.core_find_flag = 1;
		}

		printf("\n\t Obj = %d\n", obj_val);
		cout << endl;

		Lists.SP_solns_list.clear();
		for (int m = 0; m < M_num; m++) {
			int soln_val = Cplex_SP.getValue(Z_vars_list[m]);
			printf("\t Z_%d= %d\n", m + 1, soln_val);
			Lists.SP_solns_list.push_back(soln_val);
		}
		cout << endl;

		for (int t = 0; t < T_num; t++) {
			int soln_val = Cplex_SP.getValue(X_vars_list[t]);
			printf("\t X_%d = %d\n", t + 1, soln_val);
		}
		cout << endl;

		for (int t = 0; t < T_num; t++) {
			int soln_val = Cplex_SP.getValue(Y_vars_list[t]);
			printf("\t Y_%d= %d\n", t + 1, soln_val);
		}
		cout << endl;

		for (int t = 0; t < T_num; t++) {
			int soln_val = Cplex_SP.getValue(I_vars_list[t]);
			printf("\t I_%d= %d\n", t + 1, soln_val);
		}
		cout << endl;
	}

	Obj_SP.removeAllProperties();
	Obj_SP.end();
	Cplex_SP.removeAllProperties();
	Cplex_SP.end();
	Model_SP.removeAllProperties();
	Model_SP.end();

	// must end IloEnv object as the last one
	Env_SP.removeAllProperties();
	Env_SP.end();

	cout << endl;
}