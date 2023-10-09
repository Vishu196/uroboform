#pragma once
#include "NumCpp.hpp"
#include <fftw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

typedef nc::NdArray<double> ArrD;
typedef nc::NdArray<int> ArrI;


class find_edges01 
{
private:

	nc::NdArray<int>ImageArray; //2D array of int
	

	ArrI SliceImage(ArrI ImageArray);
	ArrD FindMean0(ArrI image2);
	ArrD FindMean1();
	ArrD BlackmanWindow(int n, bool sflag);
	ArrD FFT(ArrD image_window, int size);
	double Spek_Interpol(ArrD A);
	double Main_Freq(ArrD B0, int start, int stop);
	
public:

	find_edges01(nc::NdArray<int>Image);
	
	nc::NdArray<int>image2; //2D array of int
	ArrD main_d_0 = nc::NdArray<double>(1, 1);
	ArrD main_d_1 = nc::NdArray<double>(1, 1);
	ArrD Execute(ArrI ImageArray,int freq_range);
	
};