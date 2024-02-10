#pragma once
#include "grid_pos01.h"
#include <vector>

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

struct gParams
{
	int grid_height;
	int grid_width;
	int** look_up;
};

class grid_pos02
{
private:

	double* decumulateDouble(double* x, int size);
	double* Mean0R(int rows, int cols, int** array);
	double* Mean1R(int rows, int cols, int** array);
	double MeanR(int rows, double* array);
	double IntMeanR(int rows, int* mean0);
	Grid** checkGrid(Grid** grids01, int gRows, int gCols);
	vector<int> linspace(double start, double end, int num);
	struct RdBinary ReadBinary(Grid** cgrids, int** img, int x, int y);
	struct gParams grid_params(void);
	double get_d_k(Grid** cgrids, int grid_wid, int grid_ht);

public:

	grid_pos02(struct stage34 s34);
	struct stage45 Execute(void);
};

