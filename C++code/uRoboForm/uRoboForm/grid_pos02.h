#pragma once
#include "grid_pos01.h"

using namespace std;

struct stage43
{
	int** img;
	int imgRows;
	int imgCols;
	int gridRows;
	int gridCols;
	Grid** grids;
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
};

struct RdBinary
{
	int index;
	string ind_ori;
};

class grid_pos02
{
private:

	Grid** checkGrid(Grid** grids, int x, int y);
	struct RdBinary ReadBinary(Grid** cgrids, int** img, int x, int y);
	double get_d_k(Grid** cgrids, int grid_wid, int grid_ht);

public:

	grid_pos02(struct stage34 s34);
	struct stage45 Execute(void);
};

