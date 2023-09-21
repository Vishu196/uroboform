#pragma once
#include "NumCpp.hpp"
#include <fftw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

class find_edges01 
{
private:

	nc::NdArray<int>ImageArray; //2D array of int
	nc::NdArray<int>image2; //2D array of int
	nc::NdArray<double>Mean0; //1D array double-mean along rows
	nc::NdArray<double>Mean1; //1D array double-mean along cols
	nc::NdArray<double>wFun;
	nc::NdArray<double>image_window;
	//nc::NdArray<fftwf_complex> y;
	//nc::NdArray<double>median1; //1D array double

	auto SliceImage();
	auto FindMean0();
	auto FindMean1();
	auto BlackmanWindow(int n, bool sflag);
	auto FindFFT(int N, nc::NdArray<double>image_window);
	auto Spek_Interpol(nc::NdArray<double>n_g);
	

public:

	find_edges01(nc::NdArray<int>Image);
	
	nc::NdArray<double> main_d;
	nc::NdArray<double>median;

	auto FindMedian();
	auto Main_Freq(nc::NdArray<double>B);

};