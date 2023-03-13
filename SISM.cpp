/*
2022-07-24
重新恢复基本的模型
multi item single machine
NT 模型
*/
#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN

#define RC_EPS 1.0e-6 // 一个接近0的很小的数

typedef IloArray<IloNumArray> NumMatrix2; // 二维参数
typedef IloArray<IloNumVarArray> NumVarMatrix2;// 二维决策变量

int NUMBER_OF_ITEMS = 3; // 6种产品
int NUMBER_OF_PERIODS = 10; // 10个时间批

vector<vector<int>> d_matrix; // 产品的需求量

vector<int> C_list; // C_t，时间 t 的产嫩
vector<int> cX_list;
vector<int> cY_list;
vector<int> cI_list;
vector<int> tX_list; // tX_i，产品 i 的生产持续时间
vector<int> tY_list; // tY_i，产品 i 的生产启动时间

using namespace std;

void ReadData()
{
	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		int tX_value = (double)(rand() % (3 - 1 + 1)) + 1;
		int tY_value = (double)(rand() % (30 - 10 + 1)) + 10;
		int cX_value = (double)(rand() % (2 - 1 + 1)) + 1;
		int cY_value = (double)(rand() % (10 - 1 + 1)) + 1;
		int cI_value = (double)(rand() % (5 - 1 + 1)) + 1;

		tX_list.push_back(tX_value);
		tY_list.push_back(tY_value);
		cX_list.push_back(cX_value);
		cY_list.push_back(cY_value);
		cI_list.push_back(cI_value);
	}

	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		C_list.push_back(800);
	}

	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		vector<int>d_vector;
		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			int d_value = (double)(rand() % (50- 10 + 1)) + 50;
			d_vector.push_back(d_value);
		}
		d_matrix.push_back(d_vector);
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

	IloArray<IloArray<IloNumVar>> X_var_matrix =
		IloArray<IloArray<IloNumVar>>(Env_OR);
	IloArray<IloArray<IloNumVar>> Y_var_matrix =
		IloArray<IloArray<IloNumVar>>(Env_OR);
	IloArray<IloArray<IloNumVar>> I_var_matrix =
		IloArray<IloArray<IloNumVar>>(Env_OR);

	float var_min = 0;
	float var_max = IloInfinity;

	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		X_var_matrix.add(IloArray<IloNumVar>(Env_OR));
		Y_var_matrix.add(IloArray<IloNumVar>(Env_OR));
		I_var_matrix.add(IloArray<IloNumVar>(Env_OR));

		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			string X_name = "X_" +to_string(i+1) + "_" + to_string(t+1);
			string Y_name = "Y_" + to_string(i+1) + "_" + to_string(t+1);
			string I_name = "I_" + to_string(i+1) + "_" + to_string(t+1);

			X_var_matrix[i].add(IloNumVar(Env_OR, var_min, var_max, ILOINT, X_name.c_str())); // X_var_matrix[i][t]
			Y_var_matrix[i].add(IloNumVar(Env_OR, var_min, 1, ILOINT, Y_name.c_str())); // Y_var_matrix[i][t]
			I_var_matrix[i].add(IloNumVar(Env_OR, var_min, var_max, ILOINT, I_name.c_str())); // I_var_matrix[i][t]	
		}
	}

	//2、目标函数
	IloExpr obj_sum(Env_OR);
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		for (int i = 0; i < NUMBER_OF_ITEMS; i++)
		{
			obj_sum += cX_list[i] * X_var_matrix[i][t];
		}
	}

	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		for (int i = 0; i < NUMBER_OF_ITEMS; i++)
		{
			obj_sum += cY_list[i] * Y_var_matrix[i][t];
		}
	}

	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		for (int i = 0; i < NUMBER_OF_ITEMS; i++)
		{
			obj_sum += cI_list[i] * I_var_matrix[i][t];
		}
	}
	IloObjective Obj_OR = IloMinimize(Env_OR, obj_sum);
	Model_OR.add(Obj_OR);
	obj_sum.end();


	// 约束
	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			IloExpr Expr_Sum(Env_OR);
			if (t == 0)
			{
				Expr_Sum += X_var_matrix[i][t] + 0;
			}
			if (t > 0)
			{
				Expr_Sum += X_var_matrix[i][t] + I_var_matrix[i][t - 1];
			}
			Model_OR.add(Expr_Sum == d_matrix[i][t] + I_var_matrix[i][t]);
			Expr_Sum.end();
		}
	}

	// 约束
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		for (int i = 0; i < NUMBER_OF_ITEMS; i++)
		{
			IloExpr Expr_Sum(Env_OR);
			Expr_Sum += X_var_matrix[i][t];
			Model_OR.add(Expr_Sum <= C_list[i] * Y_var_matrix[i][t]);
			Expr_Sum.end();
		}
	}

	// 约束4
	for (int t = 0; t < NUMBER_OF_PERIODS; t++)
	{
		IloExpr Expr_Sum_1(Env_OR);
		IloExpr Expr_Sum_2(Env_OR);
		for (int i = 0; i < NUMBER_OF_ITEMS; i++)
		{
			Expr_Sum_1 += tY_list[i] * Y_var_matrix[i][t];
			Expr_Sum_2 += tX_list[i] * X_var_matrix[i][t];
		}
		Model_OR.add(Expr_Sum_1 + Expr_Sum_2 <= C_list[t]);
		Expr_Sum_1.end();
		Expr_Sum_2.end();
	}

	Cplex_OR.extract(Model_OR);
	Cplex_OR.solve();
	Cplex_OR.exportModel("LS初始原问题.lp");

	int Obj_value = Cplex_OR.getObjValue();

	printf("/////////// Obj ////////////\n");
	printf("目标函数 = %d\n", Obj_value);

	printf("/////////// X ////////////\n");
	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			int X_value = Cplex_OR.getValue(X_var_matrix[i][t]);
			printf("X_%d_%d = %d\n",i+1,t+1,X_value);
		}
	}
	printf("//////////// Y ///////////\n");
	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			int Y_value = Cplex_OR.getValue(Y_var_matrix[i][t]);
			printf("Y_%d_%d = %d\n",i+1,t+1, Y_value);
		}
	}
	printf("///////////// I //////////\n");
	for (int i = 0; i < NUMBER_OF_ITEMS; i++)
	{
		for (int t = 0; t < NUMBER_OF_PERIODS; t++)
		{
			int I_value = Cplex_OR.getValue(I_var_matrix[i][t]);
			printf("I_%d_%d = %d\n",i+1,t+1, I_value);
		}
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