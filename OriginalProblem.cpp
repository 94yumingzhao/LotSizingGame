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

void SolveOriginalProblem(All_Values& Values, All_Lists& Lists,int coalition_flag)
{
	int prids_num = Values.prids_num;

	IloEnv Env_OR;
	IloModel Model_OR(Env_OR);
	
	/*****************/
	//1、决策变量

	IloArray<IloNumVar> X_vars(Env_OR);
	IloArray<IloNumVar> Y_vars(Env_OR);
	IloArray<IloNumVar> I_vars(Env_OR);

	for (int t = 0; t < prids_num; t++)
	{
		string X_name = "X_" + to_string(t + 1);
		string Y_name = "Y_" + to_string(t + 1);
		string I_name = "I_" + to_string(t + 1);

		IloNumVar X_var = IloNumVar(Env_OR, 0, IloInfinity, ILOINT, X_name.c_str());
		IloNumVar Y_var = IloNumVar(Env_OR, 0, 1, ILOINT, Y_name.c_str());
		IloNumVar I_var = IloNumVar(Env_OR, 0, IloInfinity, ILOINT, I_name.c_str());

		X_vars.add(X_var);
		Y_vars.add(Y_var);
		I_vars.add(I_var);
	}

	//2、目标函数
	IloExpr obj_sum(Env_OR);
	for (int t = 0; t < prids_num; t++)
	{
		obj_sum += Lists.primal_parameters[t].c_X * X_vars[t];
	}

	for (int t = 0; t < prids_num; t++)
	{
		obj_sum += Lists.primal_parameters[t].c_Y * Y_vars[t];
	}

	for (int t = 0; t < prids_num; t++)
	{
		obj_sum += Lists.primal_parameters[t].c_I * I_vars[t];
	}

	IloObjective Obj_OR = IloMinimize(Env_OR, obj_sum);
	Model_OR.add(Obj_OR);
	obj_sum.end();

	// 约束
	for (int t = 0; t < prids_num; t++)
	{
		IloExpr con_sum(Env_OR);
		if (t == 0)
		{
			con_sum += X_vars[t] + 0;
		}

		if (t > 0)
		{
			con_sum += X_vars[t] + I_vars[t - 1];
		}
		Model_OR.add(con_sum == Lists.primal_parameters[t].d + I_vars[t]);
		con_sum.end();
	}

	// 约束
	for (int t = 0; t < prids_num; t++)
	{
		IloExpr con_sum(Env_OR);
		con_sum += X_vars[t];
		Model_OR.add(con_sum <= Values.machine_capacity * Y_vars[t]);
		con_sum.end();
	}

	// update Values.machine_capacity
	Values.machine_capacity = 0;

	printf("\n/////////// CPLEX SOLVING START ////////////\n\n");
	IloCplex Cplex_OR(Env_OR);
	Cplex_OR.extract(Model_OR);
	bool OR_flag =Cplex_OR.solve();

	if (OR_flag == 0)
	{
		printf("\n	This OR has NO FEASIBLE solns\n");
	}
	else
	{
		printf("\n	This OR has FEASIBLE solns\n");

		if (coalition_flag == 0)
		{
			Cplex_OR.exportModel("LSMM123.lp");
		}
		if (coalition_flag == 1)
		{
			Cplex_OR.exportModel("LSMM1.lp");
		}
		if (coalition_flag == 2)
		{
			Cplex_OR.exportModel("LSMM2.lp");
		}
		if (coalition_flag == 3)
		{
			Cplex_OR.exportModel("LSMM3.lp");
		}
		if (coalition_flag == 4)
		{
			Cplex_OR.exportModel("LSMM12.lp");
		}
		if (coalition_flag == 5)
		{
			Cplex_OR.exportModel("LSMM13.lp");
		}
		if (coalition_flag == 6)
		{
			Cplex_OR.exportModel("LSMM23.lp");
		}

		printf("\n/////////// CPLEX SOLVING END ////////////\n");

		int Obj_value = Cplex_OR.getObjValue();
		Lists.coalition_cost_list.push_back(Obj_value);

		printf("\n	Obj = %d\n", Obj_value);

		cout << endl;
		for (int t = 0; t < prids_num; t++)
		{
			int soln_val = Cplex_OR.getValue(X_vars[t]);
			printf("	X_%d = %d\n", t + 1, soln_val);
		}

		cout << endl;
		for (int t = 0; t < prids_num; t++)
		{
			int soln_val = Cplex_OR.getValue(I_vars[t]);
			printf("	I_%d= %d\n", t + 1, soln_val);
		}

		cout << endl;
		for (int t = 0; t < prids_num; t++)
		{
			int soln_val = Cplex_OR.getValue(Y_vars[t]);
			printf("	Y_%d= %d\n", t + 1, soln_val);
		}
	}


	Env_OR.end();
	Lists.primal_parameters.clear();

	cout << endl;
}
