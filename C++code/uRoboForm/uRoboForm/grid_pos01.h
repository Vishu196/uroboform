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

struct subPX
{
	double* max_pos;
	list<double> pres;
};

class grid_pos01
{
private:
	
	double* Mean0R(int rows, int cols, int** array);
	double* Mean1R(int rows, int cols, int** array);
	double MeanR(int rows, double* array);
	double* Bandfilter(double* x, int start, int end, int x_size);
	double* RFFT(double* x, int x_size);
	double* IRFFT(double* x, int x_size);
	double* gradient(double* x, int x_size);
	int** cutGrid(int** grid_rot, int s1, int s2);
	struct subPX subpx_max_pos(int** cutGrid, int stripeW, float px_size, string mode);
	struct subPX subpx_gauss(double* B_cut, double B_max, double B_min, double d_m);
	struct subPX subpx_parabel(double* B_cut, double B_max, double B_min, double d_m);



public:

	grid_pos01(struct stage23 s23);
	int Execute(void);
};

