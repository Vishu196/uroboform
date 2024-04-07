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

	stage45() :gridRows(0), gridCols(0), index(0), k(0.0)
	{};
};

struct RdBinary
{
	int index;
	string ind_ori;

	RdBinary() : index(0), ind_ori()
	{};
};

class grid_pos02
{
private:

	Grid** checkGrid(Grid** &grids01, int gRows, int gCols);
	vector<int> linspace(double start, double end, int num);
	struct RdBinary ReadBinary(Grid** &cgrids);
	int get_mask_pos(Grid field, int row, int col, size_t i_max);
	double calc_d_k(vector<vector<double>> lines);
	double get_d_k(Grid** &cgrids, int gRows, int gCols);

public:
	grid_pos02(struct stage34 s34);
	struct stage45 Execute(void);
};

