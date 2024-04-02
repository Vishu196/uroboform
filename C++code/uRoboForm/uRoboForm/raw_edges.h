#pragma once
#include"Evaluation.h"
#include "signal_evaluation.h"

struct stage12
{		
	Mat img;
	Mat img2;
	vector<double> mean0;
	vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage12()
	{
		img;
		img2;
		th_edge = 0;
		main_d_0 = 0.0;
		main_d_1 = 0.0;
	}
};

class raw_edges
{
	friend class grid_pos01;

private:

	Mat ImageSliceR (Mat ImageR, int n);
	double Spek_InterpolR (const vector<double> &A);
	double Main_FreqR (const vector<double> &B0, int start, int stop);
	double Calc_main_d(const vector<double> &mean0, int freq_range);

public:

	raw_edges(Mat ImageR);
	struct stage12 ExecuteR(Mat image, int freq_range);
};

