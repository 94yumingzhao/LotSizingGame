
#include "GMLS.h"
using namespace std;

void SplitString(const string& line_string, vector<string>& string_list, const string& data_string)
{
	string_list.clear();
	string::size_type pos1, pos2;
	
	pos1 = 0;
	pos2 = line_string.find(data_string);
	
	while (string::npos != pos2)
	{
		string_list.push_back(line_string.substr(pos1, pos2 - pos1));

		pos1 = pos2 + data_string.size();
		pos2 = line_string.find(data_string, pos1);
	}

	if (pos1 != line_string.length())
	{
		string_list.push_back(line_string.substr(pos1));
	}
}

void FirstReadData(All_Values& Values, All_Lists& Lists,int coalition_flag)
{
	

	int machs_num = Values.machs_num;
	int prids_num = Values.prids_num;

	ifstream file_c1; 
	ifstream file_c2;
	ifstream file_c3;

	string oneline_c1;
	string oneline_c2;
	string oneline_c3;

	vector<string> linedatas_c1;
	vector<string> linedatas_c2;
	vector<string> linedatas_c3;

	// coalition class 0
	if (coalition_flag == 0)
	{
		file_c1.open("machine01.csv");
		file_c2.open("machine02.csv");
		file_c3.open("machine03.csv");
	}

	// coalition class 1
	if (coalition_flag == 1)
	{
		file_c1.open("machine01.csv");
	}
	if (coalition_flag == 2)
	{
		file_c1.open("machine02.csv");
	}
	if (coalition_flag == 3)
	{
		file_c1.open("machine03.csv");
	}

	// coalition class 2
	if (coalition_flag == 4)
	{
		file_c1.open("machine01.csv");
		file_c2.open("machine02.csv");
	}
	if (coalition_flag == 5)
	{
		file_c1.open("machine01.csv");
		file_c2.open("machine03.csv");
	}
	if (coalition_flag == 6)
	{
		file_c1.open("machine02.csv");
		file_c2.open("machine03.csv");
	}

	// init vector
	for (int t = 0; t < prids_num; t++)
	{
		Parameter_Struct pm;
		Lists.primal_parameters.push_back(pm);
	}

	// file 1
	getline(file_c1, oneline_c1); // skip line 1, col names
	getline(file_c1, oneline_c1); // reach line 2, demand data
	SplitString(oneline_c1, linedatas_c1, ","); // split line 2

	// file 2
	getline(file_c2, oneline_c2); // skip line 1, col names
	getline(file_c2, oneline_c2); // reach line 2, demand data
	SplitString(oneline_c2, linedatas_c2, ","); // split line 2

	// file 3
	getline(file_c3, oneline_c3); // skip line 1, col names
	getline(file_c3, oneline_c3); // reach line 2, demand data
	SplitString(oneline_c3, linedatas_c3, ","); // split line 2

	// coalition class 0: grand coalition of all machines
	if (coalition_flag == 0)
	{
		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].d =
				atoi(linedatas_c1[t].c_str()) + atoi(linedatas_c2[t].c_str()) + atoi(linedatas_c3[t].c_str());
			Values.machine_capacity += Lists.primal_parameters[t].d;
		}
	}

	// coalition class 1: one machine one coalition
	if (coalition_flag == 1 || coalition_flag == 2 || coalition_flag == 3)
	{
		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].d = atoi(linedatas_c1[t].c_str());
			Values.machine_capacity+= Lists.primal_parameters[t].d;
		}
	}

	// coalition class 2: one-one coalition of machines
	if (coalition_flag == 4 || coalition_flag == 5 || coalition_flag == 6)
	{
		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].d = atoi(linedatas_c1[t].c_str()) + atoi(linedatas_c2[t].c_str());
			Values.machine_capacity += Lists.primal_parameters[t].d;
		}	
	}

	
	getline(file_c1, oneline_c1); // skip line 3, space
	getline(file_c1, oneline_c1); // skip line 4, col names
	getline(file_c1, oneline_c1); // read line 5, production cost data
	SplitString(oneline_c1, linedatas_c1, ","); // split line 5

	for (int t = 0; t < prids_num; t++)
	{
		Lists.primal_parameters[t].c_X = atoi(linedatas_c1[t].c_str()); // read line 5
	}

	getline(file_c1, oneline_c1); // skip line 6, space
	getline(file_c1, oneline_c1); // skip line 7, col names
	getline(file_c1, oneline_c1); // reach line 8, setup cost data
	SplitString(oneline_c1, linedatas_c1, ","); // split line 8

	for (int t = 0; t < prids_num; t++)
	{
		Lists.primal_parameters[t].c_Y = atoi(linedatas_c1[t].c_str()); // read line 8
	}

	getline(file_c1, oneline_c1); // skip line 9, space
	getline(file_c1, oneline_c1); // skip line 10, col names
	getline(file_c1, oneline_c1); // reach line 11, inventory cost data
	SplitString(oneline_c1, linedatas_c1, ","); // split line 11

	for (int t = 0; t < prids_num; t++)
	{
		Lists.primal_parameters[t].c_I = atoi(linedatas_c1[t].c_str()); // read line 11
	}

	cout << endl;
}

void NewReadData(All_Values& Values, All_Lists& Lists, int coalition_flag)
{
	int machs_num = Values.machs_num;
	int prids_num = Values.prids_num;

	fstream file_c1;
	fstream file_c2;
	fstream file_c3;

	string oneline_c1;
	string oneline_c2;
	string oneline_c3;

	vector<string> linedatas_c1;
	vector<string> linedatas_c2;
	vector<string> linedatas_c3;

	file_c1.open("machine01.csv");
	file_c2.open("machine02.csv");
	file_c3.open("machine03.csv");
 
	// init demand_matrix
	for (int m = 0; m < machs_num; m++)
	{
		vector<int> demand_list;
		for (int t = 0; t < prids_num; t++)
		{
			int one_demand=-1;
			demand_list.push_back(one_demand);
		}
		Lists.demand_matrix.push_back(demand_list);
	}

	// update demand_matrix
	if (coalition_flag == 10)
	{
		getline(file_c1, oneline_c1); 
		getline(file_c1, oneline_c1); 
		SplitString(oneline_c1, linedatas_c1, ",");

		getline(file_c2, oneline_c2); 
		getline(file_c2, oneline_c2); 	
		SplitString(oneline_c2, linedatas_c2, ",");

		getline(file_c3, oneline_c3); 
		getline(file_c3, oneline_c3); 		
		SplitString(oneline_c3, linedatas_c3, ","); 

		for (int m = 0; m < machs_num; m++)
		{
			for (int t = 0; t < prids_num; t++)
			{
				if (m == 0)
				{
					Lists.demand_matrix[m][t] = atoi(linedatas_c1[t].c_str());
				}
				if (m == 1)
				{
					Lists.demand_matrix[m][t] = atoi(linedatas_c2[t].c_str());
				}
				if (m == 2)
				{
					Lists.demand_matrix[m][t] = atoi(linedatas_c3[t].c_str());
				}
			}
		}
		
		// init vector<> primal_parameters
		for (int t = 0; t < prids_num; t++)
		{
			Parameter_Struct pm;
			Lists.primal_parameters.push_back(pm);
		}

		Values.machine_capacity = 1000;

		getline(file_c1, oneline_c1); // skip line 3, space
		getline(file_c1, oneline_c1); // skip line 4, col names
		getline(file_c1, oneline_c1); // read line 5, production cost data
		SplitString(oneline_c1, linedatas_c1, ","); // split line 5

		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].c_X = atoi(linedatas_c1[t].c_str()); // read line 5
		}

		getline(file_c1, oneline_c1); // skip line 6, space
		getline(file_c1, oneline_c1); // skip line 7, col names
		getline(file_c1, oneline_c1); // reach line 8, setup cost data
		SplitString(oneline_c1, linedatas_c1, ","); // split line 8

		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].c_Y = atoi(linedatas_c1[t].c_str()); // read line 8
		}

		getline(file_c1, oneline_c1); // skip line 9, space
		getline(file_c1, oneline_c1); // skip line 10, col names
		getline(file_c1, oneline_c1); // reach line 11, inventory cost data
		SplitString(oneline_c1, linedatas_c1, ","); // split line 11

		for (int t = 0; t < prids_num; t++)
		{
			Lists.primal_parameters[t].c_I = atoi(linedatas_c1[t].c_str()); // read line 11
		}
	}

	cout << endl;
}



