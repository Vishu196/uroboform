#pragma once
#include "find_edges.h"


using namespace std;

struct stage32
{
	int** img;
	int** img2;
	int imgRows;
	int imgCols;
	list<int> cut_hor;
	list<int> cut_ver;
	int cut_hor_s;
	int cut_ver_s;
};

struct stage34
{
	int** img;
	int grids; //confirm the datatype
};

class grid_pos01
{
private:
	int** flip_grid(int** grid0, int axis, int l, int w);

public:

	grid_pos01(struct stage23 s23);
	int Execute(void);
};

