#pragma once
#include "raw_edges.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include <complex>

struct stage21
{
	Mat img;
	Mat img2;
	vector<double> mean0;
	vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage21()
	{
		main_d_0 = 0.0;
		main_d_1 = 0.0;
		th_edge = 0.0;
	}
};

struct Detect_throu
{
	vector<int> through_loc;
	vector<int> cut_through;
};

struct peaks
{
	vector<int> stripes;
	vector<double> s_dic;
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
	Mat img;
	Mat img2;
	list<int> cut_hor;
	list<int> cut_ver;

	stage23()
	{
		Mat img;
		Mat img2;
		cut_hor = {};
		cut_ver = {};

	}
};

class find_edges
{
private:
	
	peaks Find_Peaks(const vector<double> &arr, double th_edge);
	indexes Line_Index(const vector<double> &mean_range_in, double th_edge,int i0,int rank);
	Detect_throu Detect_Through(const vector<double> &im_col, double th_edge);
	list<int> Delete_Edges(vector<int> cut_arr, int ideal_d);
	
public:

	find_edges(stage12 s12);
	stage23 Execute(void);
};

