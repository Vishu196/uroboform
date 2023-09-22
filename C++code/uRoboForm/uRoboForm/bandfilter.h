#pragma once
#include "NumCpp.hpp"

class bandfilter
{
public:

	bandfilter(nc::NdArray<double>x, nc::NdArray<int>limits);
	nc::NdArray<double>f_x, f_x_cut, x_cut;

	auto Find_RFFT();
	auto Find_IRFFT();
	auto Calc_Bandfilter(nc::NdArray<double>x, nc::NdArray<int>limits);


};

