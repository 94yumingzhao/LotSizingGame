// 2023-02-21 lot sizing sub problem


#include "GMLS.h"
using namespace std;

void SolveSubProblem(All_Values& Values, All_Lists& Lists) {

	int T_num = Values.T_num;
	int M_num = Values.M_num;
	int C_num = Values.C_num;

	IloEnv Env_SP;
	IloModel Model_SP(Env_SP);
	IloCplex Cplex_SP(Env_SP);

	// vars
	IloArray < IloArray<IloNumVar>> X_vars(Env_SP);
	IloArray < IloArray<IloNumVar>> Y_vars(Env_SP);
	IloArray < IloArray<IloNumVar>> I_vars(Env_SP);

	for (int m = 0; m < C_num; m++) {
		X_vars.add(IloArray<IloNumVar>(Env_SP));
		Y_vars.add(IloArray<IloNumVar>(Env_SP));
		I_vars.add(IloArray<IloNumVar>(Env_SP));

		for (int t = 0; t < T_num; t++) {

			string X_name = "X_" + to_string(m + 1) + to_string(t + 1);
			string Y_name = "Y_" + to_string(m + 1) + to_string(t + 1);
			string I_name = "I_" + to_string(m + 1) + to_string(t + 1);

			IloNumVar X_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, X_name.c_str());
			IloNumVar Y_var = IloNumVar(Env_SP, 0, 1, ILOINT, Y_name.c_str());
			IloNumVar I_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, I_name.c_str());

			X_vars[m].add(X_var);
			Y_vars[m].add(Y_var);
			I_vars[m].add(I_var);
		}
	}

	IloArray<IloNumVar> Z_vars(Env_SP);

	for (int m = 0; m < M_num; m++) {
		string Z_name = "Z_" + to_string(m + 1);
		Z_vars.add(IloNumVar(Env_SP, 0, 1, ILOINT, Z_name.c_str()));
	}

	IloArray<IloArray<IloArray<IloNumVar>>> K_Vars(Env_SP); // Z(m,n,i,t)

	for (int m = 0; m < C_num; m++) {
		K_Vars.add(IloArray < IloArray<IloNumVar>>(Env_SP));

		for (int n = 0; n < C_num; n++) {
			K_Vars[m].add(IloArray<IloNumVar>(Env_SP));

			for (int t = 0; t < T_num; t++) {
				string var_name = "K_" + to_string(m + 1) + "_" + to_string(n + 1) + "_" + to_string(t + 1);
				IloNumVar K_var = IloNumVar(Env_SP, 0, IloInfinity, ILOINT, var_name.c_str());
				K_Vars[m][n].add(K_var);
			}
		}
	}

	// obj
	IloExpr sum_obj(Env_SP);

	for (int m = 0; m < M_num; m++) {
		sum_obj += Lists.MP_solns_list[m] * Z_vars[m]; // sum W_m * Z_m
	}

	for (int m = 0; m < C_num; m++) {
		for (int t = 0; t < T_num; t++) {
			sum_obj -= Lists.primal_parameters[t].c_X * X_vars[m][t];
		}

		for (int t = 0; t < T_num; t++) {
			sum_obj -= Lists.primal_parameters[t].c_Y * Y_vars[m][t];
		}

		for (int t = 0; t < T_num; t++) {
			sum_obj -= Lists.primal_parameters[t].c_I * I_vars[m][t];
		}
	}

	for (int m = 0; m < C_num; m++) {
		for (int n = 0; n < C_num; n++) {
			if (m != n) {
				for (int t = 0; t < T_num; t++) {
					sum_obj -= Lists.primal_parameters[0].c_I * K_Vars[m][n][t];
				}
			}
		}
	}

	IloObjective Obj_SP = IloMaximize(Env_SP, sum_obj); // obj max
	Model_SP.add(Obj_SP);
	sum_obj.end();

	// number of con 1 == T
	for (int t = 0; t < T_num; t++) {
		IloExpr sum_1(Env_SP);

		for (int m = 0; m < M_num; m++) {
			sum_1 += Lists.demand_matrix[m][t] * Z_vars[m]; // sum d_mt *  Z_m
		}

		for (int m = 0; m < C_num; m++) {
			for (int n = 0; n < C_num; n++) {
				if (m != n) {
					if (t == 0) {
						Model_SP.add(X_vars[m][t] + K_Vars[m][n][t] == sum_1 + I_vars[m][t] + K_Vars[n][m][t]);
					}
					if (t > 0) {
						Model_SP.add(X_vars[m][t] + I_vars[m][t - 1] + K_Vars[m][n][t] == sum_1 + I_vars[m][t] + K_Vars[n][m][t]);
					}
				}
			}
		}
		sum_1.end();
	}

	// number of con 2 == T 
	for (int m = 0; m < C_num; m++) {
		for (int t = 0; t < T_num; t++) {
			Model_SP.add(X_vars[m][t] <= Values.machine_capacity * Y_vars[m][t]);
		}
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

		double obj_val = Cplex_SP.getObjValue();

		if (obj_val <= 0) {
			Values.core_find_flag = 1;
		}

		printf("\n\t Obj = %f\n", obj_val);
		cout << endl;

		Lists.SP_solns_list.clear();
		for (int m = 0; m < M_num; m++) {
			double soln_val = Cplex_SP.getValue(Z_vars[m]);
			printf("\t Z_%d= %f\n", m + 1, soln_val);
			Lists.SP_solns_list.push_back(soln_val);
		}
		cout << endl;

		for (int m = 0; m < C_num; m++) {
			for (int t = 0; t < T_num; t++) {
				double soln_val = Cplex_SP.getValue(X_vars[m][t]);
				printf("\t X_%d_%d = %f\n", m + 1, t + 1, soln_val);
			}
		}
		cout << endl;

		for (int m = 0; m < C_num; m++) {
			for (int t = 0; t < T_num; t++) {
				double soln_val = Cplex_SP.getValue(Y_vars[m][t]);
				printf("\t Y_%d_%d= %f\n", m + 1, t + 1, soln_val);
			}
		}
		cout << endl;

		for (int m = 0; m < C_num; m++) {
			for (int t = 0; t < T_num; t++) {
				double soln_val = Cplex_SP.getValue(I_vars[m][t]);
				printf("\t I_%d_%d= %f\n", m + 1, t + 1, soln_val);
			}
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