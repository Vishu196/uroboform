#pragma once
#include "grid_pos02.h"
#include "grid.h"
#include "constants.h"
#include"Evaluation.h"

using namespace std;

struct stage54
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	double k;
	string ind_ori;

	stage54() :gridRows(0), gridCols(0), index(0), k(0.0)
	{};
};

struct stage56
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	double k;
	string ind_ori;
	double xi;
	double zi;

	stage56() :gridRows(0), gridCols(0), index(0), k(0.0),ind_ori(),xi(0.0),zi(0.0)
	{};
};

class grid_pos03
{
private:

	vector<vector<list<int>>> grid_params(void);
	int get_mask_pos(Grid field, int row, int col, size_t i_max);
	double weighted_avg(const vector<vector<double>> &center);

public:
	grid_pos03(struct stage45 s45);
	struct stage56 Execute(void);
};

