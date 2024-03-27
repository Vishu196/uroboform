#pragma once
#include <fftw3.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct stage12
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

	stage12()
	{
		imgRows = 0;
		imgCols = 0;
		th_edge = 0;

		mean0 = 0;
		mean1 = 0;
		mean0 = new double[720];
		mean1 = new double[540];

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

		main_d_0 = 0.0;
		main_d_1 = 0.0;
	}
};

class raw_edges
{
	friend class grid_pos01;

private:

	Mat ImageSliceR (Mat ImageR, int n);
	int** Image2ArrayR (Mat ImageR2);
	double* Mean0R(int rows, int cols, int** array);
	double* Mean1R(int rows, int cols, int** array);
	double MeanR(int rows, double* array);
	double Median(int size, double* array);
	double* BlackmanWindowR (int n);
	double* FFTR (double* image_window, int size);
	double Spek_InterpolR (double* A);
	double Main_FreqR (double* B0, int start, int stop);
	
public:

	raw_edges(Mat ImageR);
	struct stage12 ExecuteR(Mat image, int freq_range);
	

	
};

