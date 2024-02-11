#pragma once
#include "grid_pos01.h"
#include <vector>
#include <algorithm>

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
	list<int>** look_up;
};

class grid_pos02
{
private:

	double* decumulateDouble(double* x, int size);
	double* Mean0R(int rows, int cols, int** array);
	double* Mean1R(int rows, int cols, int** array);
	double MeanR(int rows, double* array);
	double IntMeanR(int rows, int* mean0);
	double* insertXdouble(int size, double* arr, double x, int pos);
	Grid** checkGrid(Grid** grids01, int gRows, int gCols);
	vector<int> linspace(double start, double end, int num);
	struct RdBinary ReadBinary(Grid** cgrids, int** img, int x, int y);
	struct gParams grid_params(void);
	int get_mask_pos(Grid field, int row, int col, int i_max, int grid_wid, int grid_ht);
	float calc_d_k(list<list <int>> lines);
	double get_d_k(Grid** cgrids, int gRows, int gCols, int grid_wid, int grid_ht, double px_size);

public:

	grid_pos02(struct stage34 s34);
	struct stage45 Execute(void);
};

