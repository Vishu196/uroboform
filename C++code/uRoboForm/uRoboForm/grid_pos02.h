#pragma once
#include "NumCpp.hpp"

class grid_pos02
{
private:
	nc::NdArray<int>grids;
	auto check_grids(nc::NdArray<int>grids);
	auto read_binary(nc::NdArray<int>grids, nc::NdArray<int>image);
	auto get_d_k(nc::NdArray<int>grids, int grid_width, int grid_height);
	

public:

	int index;
	char ind_ori;
	float k;

	float find_k(float d_k, float px);
};

