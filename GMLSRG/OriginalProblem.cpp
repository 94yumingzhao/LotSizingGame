/*
2022-01-17
single item multi machine lot sizing
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include "GMLS.h"

ILOSTLBEGIN

#define RC_EPS 1.0e-6 // 一个接近0的很小的数
using namespace std;

void SolveOriginalProblem(All_Values& Values, All_Lists& Lists, int coalition_flag) {
	int prids_num = Values.prids_num;

	IloEnv Env_OP;
	IloModel Model_OP(Env_OP);

	/*****************/
	//1、决策变量

	IloArray<IloNumVar> X_vars_list(Env_OP);
	IloArray<IloNumVar> Y_vars_list(Env_OP);
	IloArray<IloNumVar> I_vars_list(Env_OP);

	for (int t = 0; t < prids_num; t++) {
		string X_name = "X_" + to_string(t + 1);
		string Y_name = "Y_" + to_string(t + 1);
		string I_name = "I_" + to_string(t + 1);

		IloNumVar X_var = IloNumVar(Env_OP, 0, IloInfinity, ILOINT, X_name.c_str());
		IloNumVar Y_var = IloNumVar(Env_OP, 0, 1, ILOINT, Y_name.c_str());
		IloNumVar I_var = IloNumVar(Env_OP, 0, IloInfinity, ILOINT, I_name.c_str());

		X_vars_list.add(X_var);
		Y_vars_list.add(Y_var);
		I_vars_list.add(I_var);
	}

	//2、目标函数
	IloExpr sum_obj(Env_OP);
	for (int t = 0; t < prids_num; t++) {
		sum_obj += Lists.primal_parameters[t].c_X * X_vars_list[t];
	}

	for (int t = 0; t < prids_num; t++) {
		sum_obj += Lists.primal_parameters[t].c_Y * Y_vars_list[t];
	}

	for (int t = 0; t < prids_num; t++) {
		sum_obj += Lists.primal_parameters[t].c_I * I_vars_list[t];
	}

	IloObjective Obj_OP = IloMinimize(Env_OP, sum_obj);
	Model_OP.add(Obj_OP);
	sum_obj.end();

	// 约束
	for (int t = 0; t < prids_num; t++) {
		IloExpr sum_1(Env_OP);
		if (t == 0) {
			sum_1 += X_vars_list[t] + 0;
		}

		if (t > 0) {
			sum_1 += X_vars_list[t] + I_vars_list[t - 1];
		}
		Model_OP.add(sum_1 == Lists.primal_parameters[t].d + I_vars_list[t]);
		sum_1.end();
	}

	// 约束
	for (int t = 0; t < prids_num; t++) {
		IloExpr sum_1(Env_OP);
		sum_1 += X_vars_list[t];
		Model_OP.add(sum_1 <= Values.machine_capacity * Y_vars_list[t]);
		sum_1.end();
	}

	// update Values.machine_capacity
	Values.machine_capacity = 0;

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_OP(Env_OP);
	Cplex_OP.extract(Model_OP);
	bool OR_flag = Cplex_OP.solve();

	if (OR_flag == 0) {
		printf("\n	This OR has NO FEASIBLE solns\n");
	}
	else {
		printf("\n	This OR has FEASIBLE solns\n");

		if (coalition_flag == 0) {
			Cplex_OP.exportModel("LSMM123.lp");
		}
		if (coalition_flag == 1) {
			Cplex_OP.exportModel("LSMM1.lp");
		}
		if (coalition_flag == 2) {
			Cplex_OP.exportModel("LSMM2.lp");
		}
		if (coalition_flag == 3) {
			Cplex_OP.exportModel("LSMM3.lp");
		}
		if (coalition_flag == 4) {
			Cplex_OP.exportModel("LSMM12.lp");
		}
		if (coalition_flag == 5) {
			Cplex_OP.exportModel("LSMM13.lp");
		}
		if (coalition_flag == 6) {
			Cplex_OP.exportModel("LSMM23.lp");
		}

		printf("\n/////////// CPLEX SOLVING END ////////////\n");

		int Obj_value = Cplex_OP.getObjValue();
		Lists.coalition_cost_list.push_back(Obj_value);

		printf("\n	Obj = %d\n", Obj_value);

		cout << endl;
		for (int t = 0; t < prids_num; t++) {
			int soln_val = Cplex_OP.getValue(X_vars_list[t]);
			printf("	X_%d = %d\n", t + 1, soln_val);
		}

		cout << endl;
		for (int t = 0; t < prids_num; t++) {
			int soln_val = Cplex_OP.getValue(I_vars_list[t]);
			printf("	I_%d= %d\n", t + 1, soln_val);
		}

		cout << endl;
		for (int t = 0; t < prids_num; t++) {
			int soln_val = Cplex_OP.getValue(Y_vars_list[t]);
			printf("	Y_%d= %d\n", t + 1, soln_val);
		}
	}


	Obj_OP.removeAllProperties();
	Obj_OP.end();
	Cplex_OP.removeAllProperties();
	Cplex_OP.end();
	Model_OP.removeAllProperties();
	Model_OP.end();
	Env_OP.removeAllProperties();
	Env_OP.end();

	Lists.primal_parameters.clear();

	cout << endl;
}
