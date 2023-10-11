#pragma once
#include "raw_edges.h"
#include <complex>


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
	int s_max;
	int s_min;
};

struct stage23
{
	int** img;
	int** img2;
	int* cut_hor;
	int* cut_ver;
};

class find_edges
{
private:
	/*
	float cond_col, cond_row_low, cond_row;
	int ideal_d;*/

	double* Bandfilter(double *x,int *limits, int x_size);
	double* RFFT(double* x, int x_size);
	double* IRFFT(double* x, int x_size);
	struct FP Find_Peaks(double *arr, int n, double th_edge);
	int* ArgSort(double* s_dic, int s_dic_size);
	int* insertXint(int size, int* arr, int x, int pos);
	double* insertXdouble(int size, double* arr, double x, int pos);
	struct LI Line_Index(double* mean_range0, int arr_size, double th_edge,int i0,int rank);
	struct DT Detect_Through(double* im_col, double th_edge);
	int* Delete_Edges(int* cut_hor, int ideal_d);

public:

	find_edges(struct stage12 s12);
	int Execute(void);
	

};

