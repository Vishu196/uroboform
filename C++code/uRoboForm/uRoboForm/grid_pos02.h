#pragma once
#include "grid_pos01.h"
#include "Evaluation.h"
#include <list>

using namespace std;

struct stage43
{
	Mat img;
	int gridRows;
	int gridCols;
	Grid** grids;

	stage43() : gridRows(0), gridCols(0)
	{};
};

struct stage45
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	double k;
	string ind_ori;
	int grid_wid;
	int grid_ht;
	vector<vector<list<int>>> look_up;
};

struct RdBinary
{
	int index;
	string ind_ori;
};

struct gParams
{
	int grid_height;
	int grid_width;
	vector<vector<list<int>>> look_up;
};

class grid_pos02
{
private:

	
	double* insertXdouble(int size, double* arr, double x, int pos);
	Grid** checkGrid(Grid** &grids01, int gRows, int gCols);
	vector<int> linspace(double start, double end, int num);
	struct RdBinary ReadBinary(Grid** &cgrids);
	struct gParams grid_params(void);
	int get_mask_pos(Grid field, int row, int col, size_t i_max, int grid_wid, int grid_ht);
	double calc_d_k(vector<vector<double>> lines);
	double get_d_k(Grid** &cgrids, int gRows, int gCols, int grid_wid, int grid_ht, double px_size);

public:

	grid_pos02(struct stage34 s34);
	struct stage45 Execute(void);
};

