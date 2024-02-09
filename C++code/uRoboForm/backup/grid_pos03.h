#pragma once
#include "grid_pos01.h"
#include "grid.h"

using namespace std;

struct stage54
{
	Grid** grids;
	int index;
	double k;
	string ind_ori;
	int grid_wid;
	int grid_ht;
};

struct stage56
{
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

	int get_mask_pos(Grid field, int grid_wid, int grid_ht, int i);
	double weighted_avg(double* center_hor);

public:
	grid_pos03(struct stage45 s45);
	struct stage56 Execute(void);
};

