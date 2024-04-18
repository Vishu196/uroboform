#pragma once
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "cqueue.h"
#include "constants.h"

struct stage12
{		
	cv::Mat img;
	cv::Mat img2;
	std::vector<double> mean0;
	std::vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage12() :main_d_0(0.0),main_d_1(0.0),th_edge(0)
	{}
};

class raw_edges
{
private:

	cv::Mat ImageSliceR (cv::Mat ImageR, int n);
	double Calc_main_d(const std::vector<double> &mean0);
	cqueue<stage12> fifo;

public:

	void ExecuteR(const cv::Mat &image);
	stage12 getNext() 
	{
		stage12 s12;
		fifo.pop(s12);
		return s12;
	}
};

