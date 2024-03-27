#pragma once
#include "grid_pos02.h"
#include "grid.h"

using namespace std;

struct stage54
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	double k;
	string ind_ori;
	int grid_wid;
	int grid_ht;
	list<int>** look_up;
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
};

class grid_pos03
{
private:

	int get_mask_pos(Grid field, int row, int col, int i_max, int grid_wid, int grid_ht);
	double MeanR(int rows, double* array);
	double weighted_avg(double* center_hor);

public:
	grid_pos03(struct stage45 s45);
	struct stage56 Execute(void);
};

