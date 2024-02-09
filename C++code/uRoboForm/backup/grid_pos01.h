#pragma once
#include "find_edges.h"
#include "grid.h"

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
	int imgRows;
	int imgCols;
	int gridRows;
	int gridCols;
	Grid** grids; 
};

struct subPX
{
	list<double> max_pos;
	list<double> pres;
};

//struct FP
//{
//	int* stripes;
//	double* s_dic;
//	int stripe_size;
//	int s_dic_size;
//};

struct MFreq
{
	double* Image_window;
	double f_g;
	double n_g;
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
	struct FP Find_Peaks(double* arr, int n, int dist);
	int** cutGrid(int** grid_rot, int x, int y);
	int* insertXint(int size, int* arr, int x, int pos);
	int* deleteXint(int size, int* arr, int pos);
	double* BlackmanWindowR(int n);
	double* FFTR(double* image_windowR, int size);
	double Spek_InterpolR(double* A);
	struct MFreq Main_FreqR(double* B0, int start, int stop);
	struct subPX subpx_max_pos(int** cutGrid,int x, int y, int stripe_width, float px_size, string mode);
	struct subPX subpx_gauss(double* B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_parabel(double* B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_phase(int** cutGrid, int x, int y);

public:
	grid_pos01(struct stage23 s23);

	struct stage34 Execute(void);
};

