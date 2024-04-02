#pragma once
#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

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
		/*int r = 1080;
	    int c = 1440;
	    vector<vector<int>> img(r, vector<int>(c));
		vector<vector<int>> img2(r/2, vector<int>(c/2));*/
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
	vector<vector<int>> Image2ArrayR (const Mat &ImageR2);
	vector<double> Mean0R(const Mat& image2);
	vector<double> Mean1R(const Mat& image2);
	double MeanR(const vector<double> &array);
	double Median(vector<double> array);
	vector<double> BlackmanWindowR (int n);
	vector<double> FFTR (const vector<double> &image_window);
	double Spek_InterpolR (const vector<double> &A);
	double Main_FreqR (const vector<double> &B0, int start, int stop);
	double Calc_main_d(const vector<double> &mean0, int freq_range);

public:

	raw_edges(Mat ImageR);
	struct stage12 ExecuteR(Mat image, int freq_range);
};

