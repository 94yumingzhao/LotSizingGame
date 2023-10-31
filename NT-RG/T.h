/*
2023-02-13
lot sizing cooperative game
*/

#pragma once

#include <fstream>
#include <string>
#include <sstream>

#include<vector>
#include <ilcplex/ilocplex.h>
using namespace std;


struct All_Values {
	int items_num = 3;
	int T_num = 6;
	int M_num = 3;

	int current_iter = 0;
	float current_branch_var_value;

	int current_bench_time = 0;
	float current_optimal_bound = -1;

	int machine_capacity = 0;

	int core_find_flag = -1;
};

struct All_Lists {

	vector<int> cost_X; // it
	vector<int> cost_Y; // gt
	vector<int> cost_I; // ft

	vector<int> usage_X; // it
	vector<int> usage_Y; // gt
	
	vector<int> period_demand;
	vector<vector<int>> demand_matrix;

	vector<double> MP_solns_list;

	vector < double > dual_prices_list;

	vector<vector<int>> X_fix;
	vector<vector<int>> Y_fix;
	vector<vector<int>> I_fix;

	vector<vector<int>> all_machine_demand;

	vector<int> single_machine_demand;

	vector<double> coalition_cost_list; // list of all possible c(S)

	vector<double> SP_solns_list; // results of master problem

	vector<vector<int>> model_matrix; // master problem matrix modeled by rows 
	vector<vector<int>> Master_Matrix_C; // master problem matrix modeled by cols

};


void FirstReadData(All_Values& Values, All_Lists& Lists, int coalition_flag);

void SolveOriginalProblem(All_Values& Values, All_Lists& Lists, int coalition_flag);

void SolveFirstMasterProblem(All_Values& Values, All_Lists& Lists);

void NewReadData(All_Values& Values, All_Lists& Lists, int coalition_flag);

void SolveSubProblem(All_Values& Values, All_Lists& Lists);

void SolveUpdateMasterProblem(All_Values& Values, All_Lists& Lists);

void RowGeneration(All_Values& Values, All_Lists& Lists);


