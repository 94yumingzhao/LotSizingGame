/*
2022-01-17
尝试实现多机组模型
single item multi machine
T 模型
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include "GMLS.h"

ILOSTLBEGIN

#define RC_EPS 1.0e-6 // 一个接近0的很小的数

typedef IloArray<IloNumArray> NumMatrix2; // 二维参数
typedef IloArray<IloNumVarArray> NumVarMatrix2;// 二维决策变量

//int NUMBER_OF_ITEMS = 3; // 6种产品
int NUMBER_OF_PERIODS = 10; // 10个时间批

vector<int> d_list; // 产品的需求量

vector<int> C_list; // C_t，时间 t 的产嫩
vector<int> cX_list;
vector<int> cY_list;
vector<int> cI_list;

using namespace std;

void ReadData()
{

	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		int cX_value = (double)(rand() % (2 - 1 + 1)) + 1;
		int cY_value = (double)(rand() % (10 - 1 + 1)) + 1;
		int cI_value = (double)(rand() % (5 - 1 + 1)) + 1;
		int d_value = (double)(rand() % (50 - 10 + 1)) + 50;
		int C_value = 400;

		cX_list.push_back(cX_value);
		cY_list.push_back(cY_value);
		cI_list.push_back(cI_value);
		d_list.push_back(d_value);
		C_list.push_back(C_value);
	}

	cout << endl;
}

void SolveOriginalProblem()
{
	IloEnv Env_OR;
	IloModel Model_OR(Env_OR);
	IloCplex Cplex_OR(Env_OR);

	/*****************/
	//1、决策变量

	IloArray<IloNumVar> X_var =IloArray<IloNumVar>(Env_OR);
	IloArray<IloNumVar> Y_var =IloArray<IloNumVar>(Env_OR);
	IloArray<IloNumVar> I_var =IloArray<IloNumVar>(Env_OR);

	float var_min = 0;
	float var_max = IloInfinity;

	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		string X_name = "X_"  + to_string(t + 1);
		string Y_name = "Y_" + to_string(t + 1);
		string I_name = "I_"  + to_string(t + 1);

		X_var.add(IloNumVar(Env_OR, var_min, var_max, ILOINT, X_name.c_str()));
		Y_var.add(IloNumVar(Env_OR, var_min, 1, ILOINT, Y_name.c_str()));
		I_var.add(IloNumVar(Env_OR, var_min, var_max, ILOINT, I_name.c_str()));
	}

	//2、目标函数
	IloExpr obj_sum(Env_OR);
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		obj_sum += cX_list[t] * X_var[t];
	}
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		obj_sum += cY_list[t] * Y_var[t];
	}
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		obj_sum += cI_list[t] * I_var[t];
	}

	IloObjective Obj_OR = IloMinimize(Env_OR, obj_sum);
	Model_OR.add(Obj_OR);
	obj_sum.end();


	// 约束
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		IloExpr Expr_Sum(Env_OR);
		if (t == 0)
		{
			Expr_Sum += X_var[t] + 0;
		}
		if (t > 0)
		{
			Expr_Sum += X_var[t] + I_var[t - 1];
		}
		Model_OR.add(Expr_Sum == d_list[t] + I_var[t]);
		Expr_Sum.end();
	}

	// 约束
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		IloExpr Expr_Sum(Env_OR);
		Expr_Sum += X_var[t];
		Model_OR.add(Expr_Sum <= C_list[t] * Y_var[t]);
		Expr_Sum.end();
	}


	Cplex_OR.extract(Model_OR);
	Cplex_OR.solve();
	Cplex_OR.exportModel("LSMM.lp");

	int Obj_value = Cplex_OR.getObjValue();

	printf("\n/////////// Obj ////////////\n");
	printf("Obj = %d\n", Obj_value);

	printf("\n/////////// X ////////////\n");
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		int X_value = Cplex_OR.getValue(X_var[t]);
		printf("X_%d = %d\n",  t + 1, X_value);
	}

	printf("\n//////////// Y ///////////\n");
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		int Y_value = Cplex_OR.getValue(Y_var[t]);
		printf("Y_%d= %d\n",  t + 1, Y_value);
	}

	printf("\n///////////// I //////////\n");
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		int I_value = Cplex_OR.getValue(I_var[t]);
		printf("I_%d= %d\n", t + 1, I_value);
	}
	Env_OR.end();
}

void SolveFirstMasterProblem() {}

void SolveSubProblem() {}

void SolveNewMasterProblem() {}

int main()
{
	ReadData();
	SolveOriginalProblem();
	system("pause");
	return 0;
}