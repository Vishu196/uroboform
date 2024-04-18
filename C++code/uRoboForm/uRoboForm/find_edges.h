#pragma once
#include "raw_edges.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"
#include "cqueue.h"
#include <complex>

struct Detect_throu
{
	std::vector<int> through_loc;
	std::vector<int> cut_through;
};

struct peaks
{
	std::vector<int> stripes;
	std::vector<double> s_dic;
};

struct indexes 
{
	double s_max;
	double s_min;

	indexes(): s_max(0.0),s_min(0.0)
	{}
};

struct stage23
{
	cv::Mat img;
	cv::Mat img2;
	std::list<int> cut_hor;
	std::list<int> cut_ver;

	stage23()
	{
		cut_hor = {};
		cut_ver = {};
	}
};

class find_edges
{
private:
	
	peaks Find_Peaks(const std::vector<double> &arr, double th_edge);
	indexes Line_Index(const std::vector<double> &mean_range_in, double th_edge,int i0,int rank);
	Detect_throu Detect_Through(const std::vector<double> &im_col, double th_edge);
	std::list<int> Delete_Edges(std::vector<int> cut_arr, int ideal_d);
	cqueue<stage23> fifo;

public:
	void Execute(const stage12 &s12);
	stage23 getNext()
	{
		stage23 s23;
		fifo.pop(s23);
		return s23;
	}
};

