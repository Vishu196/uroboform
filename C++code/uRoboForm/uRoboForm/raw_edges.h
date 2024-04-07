#pragma once
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "cqueue.h"

struct stage12
{		
	Mat img;
	Mat img2;
	vector<double> mean0;
	vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage12() :main_d_0(0.0),main_d_1(0.0),th_edge(0)
	{}
};

class raw_edges
{
	friend class grid_pos01;

private:

	Mat ImageSliceR (Mat ImageR, int n);
	double Main_FreqR (const vector<double> &B0, int start, int stop);
	double Calc_main_d(const vector<double> &mean0, int freq_range);
	cqueue<stage12> fifo;

public:

	void ExecuteR(Mat image, int freq_range);
	stage12 getNext() 
	{
		stage12 s12;
		fifo.pop(s12);
		return s12;
	}
};

