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
	double* Mean0R(int rows, int cols, int** array);
	double* Mean1R(int rows, int cols, int** array);
	double MeanR(int rows, double* array);
	double* Bandfilter(double* x, int start, int end, int x_size);
	double* RFFT(double* x, int x_size);
	double* IRFFT(double* x, int x_size);
	double* gradient(double* x, int x_size);



public:

	grid_pos01(struct stage23 s23);
	int Execute(void);
};

