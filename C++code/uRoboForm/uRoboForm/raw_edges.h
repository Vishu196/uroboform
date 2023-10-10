#pragma once
#include <fftw3.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct stage12
{
	double main_d_0;
	double main_d_1;
	int** img;
	int** img2;
	double* mean0;
	double* mean1;
};

class raw_edges
{
private:

	Mat ImageSliceR (Mat ImageR, int n);
	int** Image2ArrayR (Mat ImageR2);
	double* Mean0R (int rows, int cols, int** array);
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

