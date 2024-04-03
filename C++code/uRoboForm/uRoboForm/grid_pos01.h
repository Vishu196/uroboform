#pragma once
#include "find_edges.h"
#include "grid.h"
#include"Evaluation.h"
#include "signal_evaluation.h"

using namespace std;

struct stage32
{
	Mat img;
	Mat img2;
	list<int> cut_hor;
	list<int> cut_ver;

	stage32()
	{
		Mat img;
		Mat img2;
		cut_hor = {};
		cut_ver = {};
	}
};

struct stage34
{
	Mat img;
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
	vector<double> Image_window;
	double f_g;
	double n_g;
};

class grid_pos01
{
private:
	
	vector<double> gradient(const vector<double> x);
	struct FP Find_Peaks(double* arr, int n, int dist);
	int** cutGrid(int** grid_rot, int x, int y);
	int* insertXint(int size, int* arr, int x, int pos);
	int* deleteXint(int size, int* arr, int pos);
	struct MFreq Main_FreqR(vector<double> B0, int start, int stop);
	struct subPX subpx_max_pos(int** cutGrid,int x, int y, int stripe_width, float px_size, string mode);
	struct subPX subpx_gauss(double* B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_parabel(double* B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_phase(int** cutGrid, int x, int y);

public:
	grid_pos01(struct stage23 s23);

	struct stage34 Execute(void);
};

