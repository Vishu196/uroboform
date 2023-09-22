#pragma once
#include "NumCpp.hpp"

class grid_pos03
{
private:
	nc::NdArray<double>center_hor, center_ver, center; //1D array
	int P, mask_pos;


public:
	int get_mask_pos();
	float weighted_average(nc::NdArray<double>center_hor);
};

