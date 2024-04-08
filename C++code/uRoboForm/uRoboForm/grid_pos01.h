#pragma once
#include "find_edges.h"
#include "grid.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"
#include "cqueue.h"
#include <numeric>

using namespace std;

struct stage34
{
	Mat img;
	int gridRows;
	int gridCols;
	Grid** grids; 

	stage34() :gridRows(0), gridCols(0), grids(0)
	{}
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
	struct FP Find_Peaks(const vector<double> &arr, double dist, double prom);
	Mat cutGrid(const Mat &grid_rot);
	struct MFreq Main_FreqR(const vector<double> &B0, int start, int stop);
	struct subPX subpx_max_pos(const Mat &cutGrid,int stripe_width, double px_size, string mode);
	struct subPX subpx_gauss(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_parabel(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m);
	struct subPX subpx_phase(const Mat &cutGrid);
	cqueue<stage34> fifo;

public:

	void Execute(struct stage23 s23);
	stage34 getNext()
	{
		stage34 s34;
		fifo.pop(s34);
		return s34;
	}
};

