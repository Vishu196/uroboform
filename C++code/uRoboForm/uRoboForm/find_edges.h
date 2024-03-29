#pragma once
#include "raw_edges.h"
#include <complex>
#include <algorithm>

using namespace std;

struct stage21
{
	/*int** img;
	int** img2;
	double* mean0;
	double* mean1;*/

	vector<vector<int>> img;
	vector<vector<int>> img2;
	vector<double> mean0;
	vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage21()
	{
		//mean0 = new double[720];
		//mean1 = new double[540];
		//img = new int* [1080];;// new int[1080][1440];
		//for (int h = 0; h < 1080; h++)
		//{
		//	img[h] = new int[1440];
		//}

		//img2 = new int* [540];;// new int[1080][1440];
		//for (int h = 0; h < 540; h++)
		//{
		//	img2[h] = new int[720];
		//}

		int r = 1080;
		int c = 1440;
		vector<vector<int>> img(r, vector<int>(c));
		vector<vector<int>> img2(r / 2, vector<int>(c / 2));
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
	vector<vector<int>> img;
	vector<vector<int>> img2;
	list<int> cut_hor;
	list<int> cut_ver;

	stage23()
	{
		cut_hor = {};
		cut_ver = {};

		int r = 1080;
		int c = 1440;
		vector<vector<int>> img(r, vector<int>(c));
		vector<vector<int>> img2(r / 2, vector<int>(c / 2));

		/*img = new int* [1080];
		for (int h = 0; h < 1080; h++)
		{
			img[h] = new int[1440];
		}

		img2 = new int* [540];
		for (int h = 0; h < 540; h++)
		{
			img2[h] = new int[720];
		}*/
	}
};

class find_edges
{
	friend class grid_pos01;
private:
	
	vector<double> Bandfilter(vector<double> x,int start, int end);
	vector<double> RFFT(vector<double> x);
	vector<double> IRFFT(vector<double> x);
	struct FP Find_Peaks(vector<double> arr, double th_edge);
	vector<int> ArgSort(vector<double> s_dic);
	/*int* insertXint(int size, int* arr, int x, int pos);
	double* insertXdouble(int size, double* arr, double x, int pos);*/
	double std_dev(vector<double> arr, int start, int stop);
	vector<int> decumulateInt(vector<int> x);
	struct LI Line_Index(vector<double> mean_range_in, double th_edge,int i0,int rank);
	struct DT Detect_Through(vector<double> im_col, double th_edge);
	list<int> Delete_Edges(vector<int> cut_arr, int ideal_d);
	vector<int> deleteXint(vector<int> arr, int pos);


public:

	find_edges(struct stage12 s12);
	struct stage23 Execute(void);
	

};

