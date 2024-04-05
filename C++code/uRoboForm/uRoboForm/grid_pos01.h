#pragma once
#include "find_edges.h"
#include "grid.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include <numeric>

using namespace std;

struct stage32
{
	Mat img;
	Mat img2;
	list<int> cut_hor;
	list<int> cut_ver;

	stage32()
	{
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
	vector<double> max_pos;
	vector<double> pres;
};

struct FP
{
	vector<int> stripes;
	vector<double> s_dic;
};

struct MFreq
{
	vector<double> Image_window;
	double f_g;
	double n_g;
};

class grid_pos01
{
private:
	
	vector<double> gradient(const vector<double> &x);
	struct FP Find_Peaks(const vector<double> &arr, int dist, double prom);
	Mat cutGrid(const Mat &grid_rot);
	struct MFreq Main_FreqR(const vector<double> &B0, int start, int stop);
	struct subPX subpx_max_pos(const Mat &cutGrid,int stripe_width, float px_size, string mode);
	struct subPX subpx_gauss(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_parabel(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_phase(const Mat &cutGrid);

public:
	grid_pos01(struct stage23 s23);

	int Execute(void);
};

