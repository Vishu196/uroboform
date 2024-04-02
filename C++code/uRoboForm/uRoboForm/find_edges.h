#pragma once
#include "raw_edges.h"
#include"Evaluation.h"
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
		img;
		img2;
		main_d_0 = 0.0;
		main_d_1 = 0.0;
		th_edge = 0.0;
	}
};

struct DT
{
	vector<int> through_loc;
	vector<int> cut_through;
};

struct FP
{
	vector<int> stripes;
	vector<double> s_dic;
};

struct LI 
{
	double s_max;
	double s_min;

	LI()
	{
		s_max = 0.0;
		s_min = 0.0;
	}
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
	
	
	struct FP Find_Peaks(const vector<double> &arr, double th_edge);
	struct LI Line_Index(const vector<double> &mean_range_in, double th_edge,int i0,int rank);
	struct DT Detect_Through(const vector<double> &im_col, double th_edge);
	list<int> Delete_Edges(vector<int> cut_arr, int ideal_d);
	vector<int> deleteXint(vector<int> arr, int pos);

public:

	find_edges(struct stage12 s12);
	struct stage23 Execute(void);
};

