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
	double main_d_0;
	double main_d_1;
	double th_edge;
};

struct DT
{
	int* through_loc;
	int* cut_through;
	int cut_throu_size;
	int thro_loc_size;
};

struct FP
{
	int* stripes;
	double* s_dic;
	int stripe_size;
	int s_dic_size;
};

struct LI 
{
	double s_max;
	double s_min;
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
};

class find_edges
{
private:
	/*
	float cond_col, cond_row_low, cond_row;
	int ideal_d;*/

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
	int* Delete_Edges(int* cut_arr, int ideal_d, int arr_size);
	int* deleteXint(int size, int* arr, int pos);


public:

	find_edges(struct stage12 s12);
	struct stage23 Execute(void);
	

};

