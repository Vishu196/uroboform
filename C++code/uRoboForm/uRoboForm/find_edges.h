#pragma once
#include "raw_edges.h"
#include <complex>
#include <algorithm>

using namespace std;

struct stage21
{
	int** img;
	int** img2;
	int imgRows;
	int imgCols;
	double* mean0;
	double* mean1;
	vector<double> mean00;
	vector<double> mean11;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage21()
	{
		imgRows = 0;
		imgCols = 0;

		mean0 = new double[720];
		mean1 = new double[540];
		img = new int* [1080];;// new int[1080][1440];
		for (int h = 0; h < 1080; h++)
		{
			img[h] = new int[1440];
		}

		img2 = new int* [540];;// new int[1080][1440];
		for (int h = 0; h < 540; h++)
		{
			img2[h] = new int[720];
		}

		main_d_0 = 0.0;
		main_d_1 = 0.0;
		th_edge = 0.0;
	}
};

struct DT
{
	int* through_loc;
	int* cut_through;
	int cut_throu_size;
	int thro_loc_size;

	DT()
	{
		cut_throu_size = 0;
		thro_loc_size = 0;
	}
};

struct FP
{
	int* stripes;
	double* s_dic;
	int stripe_size;
	int s_dic_size;

	FP()
	{
		stripe_size = 0;
		s_dic_size = 0;
	}
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
	int** img;
	int** img2;
	int imgRows;
	int imgCols;
	list<int> cut_hor;
	list<int> cut_ver;
	int cut_hor_s;
	int cut_ver_s;

	stage23()
	{
		imgRows = 0;
		imgCols = 0;
		cut_hor = {};
		cut_ver = {};

		cut_hor_s = 0;
		cut_ver_s = 0;

		img = new int* [1080];
		for (int h = 0; h < 1080; h++)
		{
			img[h] = new int[1440];
		}

		img2 = new int* [540];
		for (int h = 0; h < 540; h++)
		{
			img2[h] = new int[720];
		}
	}
};

class find_edges
{
	friend class grid_pos01;
private:
	
	double* Bandfilter(double *x,int start, int end, int x_size);
	double* RFFT(double* x, int x_size);
	double* IRFFT(double* x, int x_size);
	struct FP Find_Peaks(double *arr, int n, double th_edge);
	int* ArgSort(double* s_dic, int s_dic_size);
	int* insertXint(int size, int* arr, int x, int pos);
	double* insertXdouble(int size, double* arr, double x, int pos);
	double std_dev(double* arr, int start, int stop);
	int* decumulateInt(int* x, int size);
	struct LI Line_Index(double* mean_range_in, int arr_size, double th_edge,int i0,int rank);
	struct DT Detect_Through(double* im_col, double th_edge, int size);
	list<int> Delete_Edges(int* cut_arr, int ideal_d, int arr_size);
	int* deleteXint(int size, int* arr, int pos);


public:

	find_edges(struct stage12 s12);
	struct stage23 Execute(void);
	

};

